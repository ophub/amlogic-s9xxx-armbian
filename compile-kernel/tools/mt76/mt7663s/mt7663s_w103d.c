// SPDX-License-Identifier: GPL-2.0
/* MT7663 SDIO transport constraints specific to the ZTE W103D. */

#include <linux/mmc/sdio_func.h>
#include <linux/cache.h>
#include <linux/of.h>
#include <linux/iopoll.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#include "../sdio.h"
#include "mt7615.h"
#include "mac.h"
#include "mcu.h"
#include "mt7663s_w103d.h"

#define MCU_CMD_VENDOR_CH_PRIVILEGE 0x1c
#define MCU_CMD_VENDOR_UPDATE_STA_RECORD 0x13
#define MCU_CMD_VENDOR_REMOVE_STA_RECORD 0x14

/* Mark a legacy CID as SET so mt7615_mcu_fill_msg() emits the same
 * set_query value as gen4m's wlanSendSetQueryCmd().  The CE bit is an
 * mt76-internal command descriptor flag; it is not transmitted as part of
 * the firmware CID.
 */
#define MCU_CMD_VENDOR_SET(_cid) \
	(__MCU_CMD_FIELD_CE | FIELD_PREP(__MCU_CMD_FIELD_ID, (_cid)))

/*
 * WHISR is read-clear on MT7663S.  A register-mailbox poll therefore owns
 * every interrupt bit and all enhance-mode metadata returned by that read,
 * not just the bit 16/17 ACK it is waiting for.  Keep a W103D-private pending
 * snapshot so the normal SDIO RX/TX worker can consume the asynchronous part
 * exactly as if it had read WHISR itself.
 *
 * RX metadata is a view of the packets still queued in the device, so a later
 * snapshot replaces an earlier RX view.  WTSR release counters are read-clear
 * deltas and must instead be accumulated field by field.
 */
struct mt7663s_w103d_intr_state {
	spinlock_t lock;
	struct mt7663s_intr pending;
	/* Meson descriptor DMA requires 8-byte alignment. Also isolate the
	 * DMA_FROM_DEVICE buffers from CPU-owned state at cache-line granularity.
	 * The final alignment keeps rx_num_buf out of poll_buf's last cache line.
	 */
	struct mt7663s_intr dispatch __aligned(ARCH_DMA_MINALIGN);
	struct mt7663s_intr poll_buf __aligned(ARCH_DMA_MINALIGN);
	u16 rx_num_buf[2] __aligned(ARCH_DMA_MINALIGN);
	bool pending_valid;
	bool worker_active;
	bool quota_stopping;
	bool eeprom_settle_pending;
	bool suppress_noack_completion;
	bool join_privilege_held;
	atomic_t irq_queued;
	struct delayed_work quota_work;
	struct mt76_dev *mdev;
};

struct mt7663s_w103d_poll {
	struct mt76_dev *mdev;
	bool queued;
	bool wake_worker;
	int err;
};

/* Read-only evidence of RX-tail completion delivery, reset on module load. */
static unsigned long rx_enhance_reads;
static unsigned long rx_enhance_tx_reports;
static unsigned long rx_enhance_pse;
static unsigned long rx_enhance_ple;
static unsigned long rx_enhance_errors;
module_param(rx_enhance_reads, ulong, 0444);
module_param(rx_enhance_tx_reports, ulong, 0444);
module_param(rx_enhance_pse, ulong, 0444);
module_param(rx_enhance_ple, ulong, 0444);
module_param(rx_enhance_errors, ulong, 0444);

bool mt7663s_w103d_active(void)
{
	return of_machine_is_compatible("zte,w103d");
}

int mt7663s_w103d_mcu_restart(struct mt76_dev *mdev)
{
	return mt76_mcu_send_msg(mdev, MCU_CMD(RESTART_DL_REQ), NULL, 0, true);
}

static struct mt7663s_w103d_intr_state *
mt7663s_w103d_intr_state(struct mt76_dev *mdev)
{
	return mdev->sdio.intr_data;
}

static void mt7663s_w103d_quota_work(struct work_struct *work);

int mt7663s_w103d_init_intr(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state;

	state = devm_kzalloc(mdev->dev, sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	spin_lock_init(&state->lock);
	atomic_set(&state->irq_queued, 0);
	state->mdev = mdev;
	INIT_DELAYED_WORK(&state->quota_work, mt7663s_w103d_quota_work);
	mdev->sdio.intr_data = state;

	dev_info(mdev->dev,
		 "W103D: preserving complete read-clear WHISR snapshots\n");

	return 0;
}

static u32 mt7663s_w103d_add_tx_count(u32 old, u32 new)
{
	u32 low, high;

	low = min_t(u32, (old & 0xffff) + (new & 0xffff), U16_MAX);
	high = min_t(u32, (old >> 16) + (new >> 16), U16_MAX);

	return low | (high << 16);
}

static bool
mt7663s_w103d_snapshot_has_work(const struct mt7663s_intr *snapshot)
{
	u32 async_isr = snapshot->isr &
		~(H2D_SW_INT_READ | H2D_SW_INT_WRITE);
	int i;

	if (async_isr || snapshot->rx.num[0] || snapshot->rx.num[1])
		return true;

	for (i = 0; i < ARRAY_SIZE(snapshot->tx.wtqcr); i++)
		if (snapshot->tx.wtqcr[i])
			return true;

	return false;
}

static void
mt7663s_w103d_merge_snapshot(struct mt7663s_intr *pending,
			     const struct mt7663s_intr *snapshot)
{
	u32 async_isr = snapshot->isr &
		~(H2D_SW_INT_READ | H2D_SW_INT_WRITE);
	int i;

	pending->isr |= async_isr;

	for (i = 0; i < ARRAY_SIZE(pending->tx.wtqcr); i++) {
		if (snapshot->tx.wtqcr[i])
			pending->tx.wtqcr[i] =
				mt7663s_w103d_add_tx_count(pending->tx.wtqcr[i],
							   snapshot->tx.wtqcr[i]);
	}

	for (i = 0; i < ARRAY_SIZE(pending->rx.num); i++) {
		u32 rx_done = i ? WHIER_RX1_DONE_INT_EN :
					WHIER_RX0_DONE_INT_EN;

		if (!(async_isr & rx_done) && !snapshot->rx.num[i])
			continue;

		pending->rx.num[i] = snapshot->rx.num[i];
		memcpy(pending->rx.len[i], snapshot->rx.len[i],
		       sizeof(pending->rx.len[i]));
	}

	/* Receive mailboxes accompany D2H software interrupt bits.  Keep the
	 * newest complete pair; they are values, so OR-merging would corrupt
	 * their meaning.
	 */
	if (async_isr & WHIER_D2H_SW_INT)
		memcpy(pending->rec_mb, snapshot->rec_mb,
		       sizeof(pending->rec_mb));
}

static void
mt7663s_w103d_queue_snapshot(struct mt7663s_w103d_poll *poll,
			     const struct mt7663s_intr *snapshot)
{
	struct mt7663s_w103d_intr_state *state;
	unsigned long flags;

	if (!mt7663s_w103d_snapshot_has_work(snapshot))
		return;

	state = mt7663s_w103d_intr_state(poll->mdev);
	spin_lock_irqsave(&state->lock, flags);
	if (!state->pending_valid) {
		state->pending = *snapshot;
		state->pending.isr &=
			~(H2D_SW_INT_READ | H2D_SW_INT_WRITE);
		state->pending_valid = true;
	} else {
		mt7663s_w103d_merge_snapshot(&state->pending, snapshot);
	}
	poll->wake_worker |= !state->worker_active;
	spin_unlock_irqrestore(&state->lock, flags);
	poll->queued = true;
}

static u32 mt7663s_w103d_read_whisr_snapshot(void *data)
{
	struct mt7663s_w103d_poll *poll = data;
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(poll->mdev);
	struct mt76_sdio *sdio = &poll->mdev->sdio;

	poll->err = sdio_readsb(sdio->func, &state->poll_buf, MCR_WHISR,
				 sizeof(state->poll_buf));
	if (poll->err)
		return 0;

	mt7663s_w103d_queue_snapshot(poll, &state->poll_buf);

	return state->poll_buf.isr;
}

/* ========================================================================
 * V62 port: Mailbox timeout watchdog and stale response purge.
 *
 * The vendor gen4m driver enforces a 200 ms mailbox timeout with 1-second
 * backoff after each timeout to prevent mailbox storms.  When a timeout
 * is detected, stale MCU responses are purged to avoid contaminating the
 * next command exchange.
 * ======================================================================== */

#define W103D_MAILBOX_TIMEOUT_MS	200
#define W103D_MAILBOX_BACKOFF_MS	1000

static void mt7663s_w103d_mailbox_backoff(struct mt76_dev *mdev)
{
	/* Back off for 1 second (V62 storm protection) after releasing
	 * the shared SDIO host, and purge stale MCU responses so the
	 * next exchange starts clean without starving Bluetooth.
	 */
	msleep(W103D_MAILBOX_BACKOFF_MS);
	mt7663s_w103d_purge_stale_responses(mdev);
	dev_warn_ratelimited(mdev->dev,
		"W103D: mailbox timeout after %d ms, 1s backoff\n",
		W103D_MAILBOX_TIMEOUT_MS);
}

/* Poll step and timeout for the register-mailbox ACK.  The timeout is a
 * multiple of one sdio_readsb, matching the vendor's 200 ms cadence.  A
 * separate "done" flag breaks out of read_poll_timeout on poll error while
 * still honouring the full 200 ms storm-protection window.
 */
static int mt7663s_w103d_wait_mailbox(struct mt76_dev *mdev, u32 ack,
				      bool *wake_worker)
{
	const int poll_us = 50;
	const int timeout_us = W103D_MAILBOX_TIMEOUT_MS * 1000;
	struct mt7663s_w103d_poll poll;
	u32 status;
	int err;

	memset(&poll, 0, sizeof(poll));
	poll.mdev = mdev;

	/* Each read fetches the complete enhance-mode block, because reading
	 * only the first dword would clear and discard RX lengths and TX
	 * release counters.
	 */
	err = read_poll_timeout(mt7663s_w103d_read_whisr_snapshot, status,
				poll.err || (status & ack), poll_us,
				timeout_us, false, &poll);

	*wake_worker |= poll.queued && poll.wake_worker;

	if (err && !poll.err)
		return -ETIMEDOUT;

	if (!err && poll.err)
		err = poll.err;

	return err;
}

static void mt7663s_w103d_fill_intr(struct mt76s_intr *intr,
				    struct mt7663s_intr *snapshot,
				    struct mt7663s_w103d_intr_state *state)
{
	int i;

	intr->isr = snapshot->isr;
	intr->rec_mb = snapshot->rec_mb;
	intr->tx.wtqcr = snapshot->tx.wtqcr;
	intr->rx.num = state->rx_num_buf;
	for (i = 0; i < ARRAY_SIZE(snapshot->rx.num); i++) {
		state->rx_num_buf[i] = snapshot->rx.num[i];
		if (snapshot->rx.num[i]) {
			if (i == 0)
				intr->isr |= WHIER_RX0_DONE_INT_EN;
			else
				intr->isr |= WHIER_RX1_DONE_INT_EN;
		}
		intr->rx.len[i] = snapshot->rx.len[i];
	}
}

/* Caller owns the SDIO host for the entire status/RX transaction. The old
 * snapshot's completion counters must be consumed before replacing it.
 */
int mt7663s_w103d_rx_enhance(struct mt76_dev *mdev,
			   struct mt76s_intr *intr, const void *data)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	struct mt7663s_intr next;
	u32 *tx = next.tx.wtqcr;
	int i;

	BUILD_BUG_ON(sizeof(next) != W103D_RX_ENHANCE_SIZE);
	memcpy(&next, data, sizeof(next));
	for (i = 0; i < 2; i++) {
		if (next.rx.num[i] > 16) {
			rx_enhance_errors++;
			dev_err_ratelimited(mdev->dev,
				"W103D: invalid RX enhance count %u/%u\n",
				next.rx.num[0], next.rx.num[1]);
			return -EPROTO;
		}
	}
	rx_enhance_reads++;
	rx_enhance_tx_reports += !!(tx[0] | tx[1] | tx[2] | tx[3] |
				   tx[4] | tx[5] | tx[6] | tx[7]);
	rx_enhance_pse += (tx[0] & 0xffff) + (tx[0] >> 16) +
			  (tx[1] & 0xffff) + (tx[1] >> 16);
	rx_enhance_ple += (tx[2] >> 16) + (tx[3] & 0xffff) +
			  (tx[3] >> 16) + (tx[4] & 0xffff);
	state->dispatch = next;
	mt7663s_w103d_fill_intr(intr, &state->dispatch, state);
	return 0;
}

void mt7663s_w103d_save_intr(struct mt76_dev *mdev,
			   struct mt76s_intr *intr)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	struct mt7663s_w103d_poll poll = { .mdev = mdev };

	/* RX-tail state is newer than the consumed snapshot, including zero
 * queue lengths. Publish it before releasing the host to mailbox readers.
 */
	state->dispatch.isr = intr->isr;
	mt7663s_w103d_queue_snapshot(&poll, &state->dispatch);
}

int mt7663s_w103d_parse_intr(struct mt76_dev *mdev,
			     struct mt76s_intr *intr)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	struct mt76_sdio *sdio = &mdev->sdio;
	unsigned long flags;
	bool pending;
	int err = 0;

	spin_lock_irqsave(&state->lock, flags);
	pending = state->pending_valid;
	if (pending) {
		state->dispatch = state->pending;
		memset(&state->pending, 0, sizeof(state->pending));
		state->pending_valid = false;
	}
	spin_unlock_irqrestore(&state->lock, flags);

	if (!pending) {
		sdio_claim_host(sdio->func);
		err = sdio_readsb(sdio->func, &state->dispatch, MCR_WHISR,
				  sizeof(state->dispatch));
		sdio_release_host(sdio->func);
		if (err) {
			atomic_set(&state->irq_queued, 0);
			return err;
		}
	}
	atomic_set(&state->irq_queued, 0);

	mt7663s_w103d_fill_intr(intr, &state->dispatch, state);
	return 0;
}

void mt7663s_w103d_txrx_worker_enter(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	unsigned long flags;

	spin_lock_irqsave(&state->lock, flags);
	state->worker_active = true;
	spin_unlock_irqrestore(&state->lock, flags);
}

bool mt7663s_w103d_txrx_worker_next(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	unsigned long flags;
	bool pending;

	sdio_claim_host(mdev->sdio.func);
	spin_lock_irqsave(&state->lock, flags);
	pending = state->pending_valid;
	state->worker_active = false;
	spin_unlock_irqrestore(&state->lock, flags);
	if (!pending && !test_bit(MT76_MCU_RESET, &mdev->phy.state) &&
	    !test_bit(MT76_STATE_SUSPEND, &mdev->phy.state))
		sdio_writeb(mdev->sdio.func, WHLPCR_INT_EN_SET, MCR_WHLPCR, NULL);
	sdio_release_host(mdev->sdio.func);

	return pending;
}

void mt7663s_w103d_pm_ref_failed(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);

	if (state)
		atomic_set(&state->irq_queued, 0);
}

bool mt7663s_w103d_txqs_empty(struct mt76_dev *mdev)
{
	struct mt76_queue *q;
	int i;

	for (i = 0; i <= MT_TXQ_PSD + 1; i++) {
		if (i <= MT_TXQ_PSD)
			q = mdev->phy.q_tx[i];
		else
			q = mdev->q_mcu[MT_MCUQ_WM];

		if (q && q->first != q->head)
			return false;
	}

	return true;
}

static void mt7663s_w103d_quota_work(struct work_struct *work)
{
	struct mt7663s_w103d_intr_state *state =
		container_of(work, struct mt7663s_w103d_intr_state,
			     quota_work.work);
	struct mt76_dev *mdev = state->mdev;

	if (!mdev || READ_ONCE(state->quota_stopping))
		return;

	if (test_bit(MT76_MCU_RESET, &mdev->phy.state) ||
	    test_bit(MT76_STATE_SUSPEND, &mdev->phy.state))
		return;

	if (!mt7663s_w103d_txqs_empty(mdev))
		mt76_worker_schedule(&mdev->sdio.txrx_worker);
}

void mt7663s_w103d_arm_quota_watchdog(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state = mt7663s_w103d_intr_state(mdev);
	unsigned long flags;

	if (!state)
		return;

	/* quota_work itself wakes txrx_worker.  On a busy system that worker can
	 * reach this point before the currently executing quota_work has returned.
	 * schedule_delayed_work() then refuses to queue an already-running work
	 * item and permanently strands packets after the next quota exhaustion.
	 * mod_delayed_work() guarantees that the watchdog is pending again even
	 * in that overlap, matching the vendor driver's persistent WTSR polling.
	 */
	/* Serialize the decision and enqueue with deinit: a flag check alone
	 * would allow a producer to enqueue after cancel_delayed_work_sync().
	 */
	spin_lock_irqsave(&state->lock, flags);
	if (!state->quota_stopping)
		mod_delayed_work(system_wq, &state->quota_work,
				 msecs_to_jiffies(W103D_QUOTA_WATCHDOG_MS));
	spin_unlock_irqrestore(&state->lock, flags);
}

void mt7663s_w103d_cancel_quota_watchdog(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state = mt7663s_w103d_intr_state(mdev);

	if (!state)
		return;

	cancel_delayed_work(&state->quota_work);
}

void mt7663s_w103d_cancel_quota_watchdog_sync(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state = mt7663s_w103d_intr_state(mdev);

	if (!state)
		return;

	cancel_delayed_work_sync(&state->quota_work);
}

void mt7663s_w103d_deinit_intr(struct mt76_dev *mdev)
{
	struct mt7663s_w103d_intr_state *state = mt7663s_w103d_intr_state(mdev);
	unsigned long flags;

	if (!state)
		return;

	spin_lock_irqsave(&state->lock, flags);
	WRITE_ONCE(state->quota_stopping, true);
	spin_unlock_irqrestore(&state->lock, flags);
	/* Never wait under state->lock: a running callback can wake a producer. */
	cancel_delayed_work_sync(&state->quota_work);
}

static bool mt7663s_w103d_tx_quota_empty(struct mt76_dev *mdev)
{
	struct mt76_sdio *sdio = &mdev->sdio;
	bool data_queued = false;
	bool mcu_queued = false;
	int i;

	for (i = 0; i <= MT_TXQ_PSD; i++) {
		if (mdev->phy.q_tx[i] &&
		    mdev->phy.q_tx[i]->first != mdev->phy.q_tx[i]->head) {
			data_queued = true;
			break;
		}
	}
	if (mdev->q_mcu[MT_MCUQ_WM])
		mcu_queued = mdev->q_mcu[MT_MCUQ_WM]->first !=
			     mdev->q_mcu[MT_MCUQ_WM]->head;

	return (data_queued &&
		(sdio->sched.pse_data_quota < W103D_TX_PAGE_PER_FRAME ||
		 !sdio->sched.ple_data_quota)) ||
	       (mcu_queued && !sdio->sched.pse_mcu_quota);
}

static int mt7663s_w103d_refill_quota(struct mt76_sdio *sdio, u32 *data)
{
	u32 ple[] = {
		FIELD_GET(TXQ_CNT_L, data[4]), FIELD_GET(TXQ_CNT_H, data[3]),
		FIELD_GET(TXQ_CNT_L, data[3]), FIELD_GET(TXQ_CNT_H, data[2]),
	};
	u32 pse[] = {
		FIELD_GET(TXQ_CNT_H, data[1]), FIELD_GET(TXQ_CNT_L, data[1]),
		FIELD_GET(TXQ_CNT_H, data[0]), FIELD_GET(TXQ_CNT_L, data[0]),
	};
	u32 pse_mcu = FIELD_GET(TXQ_CNT_L, data[2]);
	u32 pse_total = 0, ple_total = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(pse); i++) {
		pse_total += pse[i];
		ple_total += ple[i];
		sdio->sched.pse_data_quota += pse[i];
		sdio->sched.ple_data_quota += ple[i];
	}
	sdio->sched.pse_mcu_quota += pse_mcu;

	return pse_total + ple_total + pse_mcu;
}

static int mt7663s_w103d_read_tx_quota(struct mt76_dev *mdev)
{
	struct mt76_sdio *sdio = &mdev->sdio;
	u32 wtqcr[8];
	int i, credits, err = 0;

	/* Check WTQCR once without sleeping so that pending RX traffic
	 * (including TCP ACKs) is never starved in the worker.
	 */
	memset(wtqcr, 0, sizeof(wtqcr));
	sdio_claim_host(sdio->func);
	for (i = 0; i < ARRAY_SIZE(wtqcr); i++) {
		u32 value = sdio_readl(sdio->func, MCR_WTQCR(i), &err);

		if (err)
			break;
		/* sdio_readl returns ~0 on failure, which is not a credit count. */
		wtqcr[i] = value;
	}
	sdio_release_host(sdio->func);

	/* WTQCR is read-clear. Even if a bus error occurred on later registers,
	 * refill any non-zero credits already read from preceding registers so
	 * credits are never leaked.
	 */
	credits = mt7663s_w103d_refill_quota(sdio, wtqcr);
	if (err)
		dev_warn_ratelimited(mdev->dev, "W103D: WTQCR%d read failed: %d\n",
				     i, err);

	return err ? err : credits;
}

int mt7663s_w103d_poll_tx_quota(struct mt76_dev *mdev)
{
	if (!mt7663s_w103d_tx_quota_empty(mdev))
		return 0;

	return mt7663s_w103d_read_tx_quota(mdev);
}

void mt7663s_w103d_txrx_worker(struct mt76_dev *mdev)
{
	mt7663s_w103d_cancel_quota_watchdog(mdev);
	mt7663s_w103d_poll_tx_quota(mdev);
	mt7663s_w103d_txrx_worker_core(mdev);
}

static void mt7663s_w103d_mac_work(struct work_struct *work)
{
	struct mt76_phy *mphy;
	struct mt7615_phy *phy;
	unsigned long timeout;

	mphy = container_of(work, struct mt76_phy, mac_work.work);
	phy = mphy->priv;

	/* The MT7663 vendor HIF does not issue mt7615's 100 ms survey/MIB/SCS
	 * register sweep.  On W103D that sweep overloads the CONNAC1 mailbox
	 * and holds the shared SDIO host, including Bluetooth.  TX completion
	 * cleanup is the only generic mac_work function required here.
	 */
	mt76_tx_status_check(mphy->dev, false);

	timeout = mt7615_get_macwork_timeout(phy->dev);
	ieee80211_queue_delayed_work(mphy->hw, &mphy->mac_work, timeout);
}

/* ========================================================================
 * EEPROM settle + stale response purge.
 *
 * The vendor gen4m driver delays 1600 ms after the EEPROM command and
 * purges stale MCU responses before touching MAC registers.  The mt7663s
 * transport cannot insert that delay inside mt7615_init_work(), so it marks
 * the completed EEPROM exchange and performs the delay before the next MCU
 * command, after the outer response wait has consumed the EEPROM response.
 * ======================================================================== */

void mt7663s_w103d_init_mac_work(struct mt7615_dev *dev)
{
	struct mt76_phy *mphy = &dev->mphy;

	if (!of_machine_is_compatible("zte,w103d"))
		return;

	/*
	 * Replace the periodic mac_work callback with a lightweight version
	 * that skips the vendor HIF's 100 ms survey/MIB/SCS register sweep.
	 * On W103D that sweep overloads the CONNAC1 mailbox and holds the
	 * shared SDIO host, including Bluetooth.  mac_work is a delayed_work
	 * in struct mt76_phy, so re-initializing it here is type-safe.
	 *
	 * EEPROM settle is deferred by mt7663s_w103d_mcu_send_message() until
	 * after EFUSE_BUFFER_MODE's response has been consumed.
	 */
	INIT_DELAYED_WORK(&mphy->mac_work, mt7663s_w103d_mac_work);
	ieee80211_queue_delayed_work(mphy->hw, &mphy->mac_work,
				     mt7615_get_macwork_timeout(dev));

	dev_info(dev->mt76.dev,
		 "W103D: mac_work replaced with lightweight TX-only callback\n");
}

/* ========================================================================
 * Internal MAC register filter.
 * ======================================================================== */

static bool mt7663s_w103d_is_internal_mac_reg(u32 offset)
{
	/* MT7663 N9 firmware does not serve internal MAC/PHY/WTBL registers
	 * (0x82000000 - 0x82ffffff) via SDIO register mailbox in running mode.
	 * WTBL and MAC state are managed internally by N9 via MCU commands.  RFCR
	 * and RFCR1 are the two bounded exceptions required to admit scan and
	 * Block Ack action traffic.
	 */
	if (offset == 0x820f5000 || offset == 0x820f5004)
		return false;

	return (offset >= 0x82000000 && offset < 0x83000000);
}

u32 mt7663s_w103d_reg_rr(struct mt76_dev *mdev, u32 offset)
{
	struct sdio_func *func = mdev->sdio.func;
	u32 irq_enable = 0, value = ~0;
	bool wake_worker = false;
	int err;

	if (mt7663s_w103d_is_internal_mac_reg(offset))
		return 0;

	sdio_claim_host(func);
	irq_enable = sdio_readb(func, MCR_WHLPCR, &err);
	if (err)
		goto out;
	irq_enable &= WHLPCR_INT_EN_SET;
	sdio_writeb(func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &err);
	if (err)
		goto out;

	sdio_writel(func, offset, MCR_H2DSM0R, &err);
	if (err)
		goto out;

	sdio_writel(func, H2D_SW_INT_READ, MCR_WSICR, &err);
	if (err)
		goto out;

	err = mt7663s_w103d_wait_mailbox(mdev, H2D_SW_INT_READ,
					  &wake_worker);
	if (err)
		goto out;

	/* WHISR is configured for read-clear on MT76_CONNAC_SDIO.  The
	 * successful poll above already acknowledged bit 16; writing that bit
	 * back is the write-1-clear sequence and does not match the MT7663
	 * vendor protocol.  The mt76 firmware leaves asynchronous information
	 * in D2HRM0R, so retain the upstream CONNAC1 H2DSM0R request check.
	 */
	value = sdio_readl(func, MCR_H2DSM0R, &err);
	if (err || value != offset) {
		value = ~0;
		goto out;
	}

	value = sdio_readl(func, MCR_D2HRM1R, &err);
	if (err)
		value = ~0;

out:
	sdio_writeb(func, irq_enable, MCR_WHLPCR, NULL);
	sdio_release_host(func);
	if (err == -ETIMEDOUT)
		mt7663s_w103d_mailbox_backoff(mdev);
	if (wake_worker)
		mt76_worker_schedule(&mdev->sdio.txrx_worker);
	if (err)
		dev_err_ratelimited(mdev->dev,
			"W103D mailbox read 0x%08x failed: %d\n", offset, err);
	return value;
}

void mt7663s_w103d_reg_wr(struct mt76_dev *mdev, u32 offset, u32 value)
{
	struct sdio_func *func = mdev->sdio.func;
	u32 address, irq_enable = 0;
	bool wake_worker = false;
	int err;

	if (mt7663s_w103d_is_internal_mac_reg(offset))
		return;

	sdio_claim_host(func);
	irq_enable = sdio_readb(func, MCR_WHLPCR, &err);
	if (err)
		goto out;
	irq_enable &= WHLPCR_INT_EN_SET;
	sdio_writeb(func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &err);
	if (err)
		goto out;

	sdio_writel(func, offset, MCR_H2DSM0R, &err);
	if (err)
		goto out;

	sdio_writel(func, value, MCR_H2DSM1R, &err);
	if (err)
		goto out;

	sdio_writel(func, H2D_SW_INT_WRITE, MCR_WSICR, &err);
	if (err)
		goto out;

	err = mt7663s_w103d_wait_mailbox(mdev, H2D_SW_INT_WRITE,
					  &wake_worker);
	if (err)
		goto out;

	/* Reading WHISR in the poll acknowledged bit 17 in read-clear mode. */
	address = sdio_readl(func, MCR_H2DSM0R, &err);
	if (!err && address != offset)
		err = -EIO;

out:
	sdio_writeb(func, irq_enable, MCR_WHLPCR, NULL);
	sdio_release_host(func);
	if (err == -ETIMEDOUT)
		mt7663s_w103d_mailbox_backoff(mdev);
	if (wake_worker)
		mt76_worker_schedule(&mdev->sdio.txrx_worker);
	if (err)
		dev_err_ratelimited(mdev->dev,
			"W103D mailbox write 0x%08x failed: %d\n", offset, err);
}

/* ========================================================================
 * V59 port: MCU command no-ACK detection and wait_resp override.
 *
 * The vendor gen4m driver uses a per-command fgNeedResp flag.  On W103D
 * with v3 firmware, UNI SET commands (DEVINFO, BSSINFO, STA_REC, WTBL
 * updates) must be sent without waiting for a response to avoid deadlocking
 * the synchronous SDIO send path.  EFUSE_BUFFER_MODE is the exception.
 * ======================================================================== */

static bool mt7663s_w103d_cmd_no_ack(int cmd)
{
	u8 ext_cid = FIELD_GET(__MCU_CMD_FIELD_EXT_ID, cmd);

	if (cmd & __MCU_CMD_FIELD_QUERY)
		return false;

	/* On MT7663 (like vendor gen4m driver fgNeedResp = FALSE), runtime UNI
	 * SET commands (DEVINFO_UPDATE, BSSINFO_UPDATE, etc.) are fire-and-forget.
	 */
	if (cmd & __MCU_CMD_FIELD_UNI)
		return true;
	/* MT7663 legacy firmware treats ordinary extended SET commands as
	 * fire-and-forget.  EFUSE_BUFFER_MODE is the one initialization SET
	 * whose completion status is required before continuing.
	 */
	return ext_cid && ext_cid != MCU_EXT_CMD_EFUSE_BUFFER_MODE;
}

/* The Meson G12A SRAM path cannot carry the stock 1608-byte EEPROM frame. */
static void mt7663s_w103d_select_eeprom_source(struct mt76_dev *mdev,
					       struct sk_buff *skb)
{
	const size_t hdr_len = 4;
	const u16 max_eep_len = 1464;
	u16 eep_len;

	if (skb->len < hdr_len)
		return;

	eep_len = le16_to_cpu(*(__le16 *)(skb->data + 2));
	if (eep_len <= max_eep_len || skb->len < hdr_len + eep_len)
		return;

	/* Do not truncate the tail: it contains board calibration bytes used by
	 * 5 GHz TX.  Ask the firmware to read the complete on-chip eFuse instead;
	 * this is the standard zero-length EE_MODE_EFUSE request used by newer
	 * connac drivers and fits in one small SDIO transaction.
	 */
	skb->data[0] = EE_MODE_EFUSE;
	*(__le16 *)(skb->data + 2) = 0;
	skb_trim(skb, hdr_len);

	dev_info_ratelimited(mdev->dev,
			     "W103D: using firmware eFuse source for complete calibration\n");
}

static void mt7663s_w103d_complete_no_ack(struct mt76_dev *mdev, int cmd,
						  u8 seq)
{
	struct mt7615_mcu_rxd *rxd;
	struct sk_buff *skb;
	size_t len = sizeof(*rxd);

	if (cmd & __MCU_CMD_FIELD_UNI)
		len += sizeof(struct mt76_connac_mcu_uni_event);

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return;

	skb_put_zero(skb, len);
	rxd = (struct mt7615_mcu_rxd *)skb->data;
	rxd->seq = seq;
	if (cmd & __MCU_CMD_FIELD_UNI) {
		struct mt76_connac_mcu_uni_event *event =
			(struct mt76_connac_mcu_uni_event *)(rxd + 1);

		event->cid = FIELD_GET(__MCU_CMD_FIELD_ID, cmd);
	}

	mt76_mcu_rx_event(mdev, skb);
}

static int mt7663s_w103d_add_dev_info(struct mt7615_phy *phy,
					      struct ieee80211_vif *vif,
					      bool enable)
{
	struct mt7615_vif *mvif = (struct mt7615_vif *)vif->drv_priv;
	struct mt7615_dev *dev = phy->dev;
	struct {
		struct {
			u8 omac_idx;
			u8 band_idx;
			__le16 tlv_num;
			u8 is_tlv_append;
			u8 rsv[3];
		} __packed hdr;
		struct {
			__le16 tag;
			__le16 len;
			u8 active;
			u8 band_idx;
			u8 omac_addr[ETH_ALEN];
		} __packed tlv;
	} __packed data = {
		.hdr = {
			.omac_idx = mvif->mt76.omac_idx,
			.band_idx = mvif->mt76.band_idx,
			.tlv_num = cpu_to_le16(1),
			.is_tlv_append = 1,
		},
		.tlv = {
			.tag = cpu_to_le16(DEV_INFO_ACTIVE),
			.len = cpu_to_le16(sizeof(data.tlv)),
			.active = enable,
			.band_idx = mvif->mt76.band_idx,
		},
	};

	memcpy(data.tlv.omac_addr, vif->addr, ETH_ALEN);
	return mt76_mcu_send_msg(&dev->mt76, MCU_EXT_CMD(DEV_INFO_UPDATE),
				 &data, sizeof(data), true);
}

static int mt7663s_w103d_add_bss_info(struct mt7615_phy *phy,
					      struct ieee80211_vif *vif,
					      struct ieee80211_sta *sta,
					      bool enable)
{
	struct mt7615_vif *mvif = (struct mt7615_vif *)vif->drv_priv;
	struct mt7615_dev *dev = phy->dev;
	struct sk_buff *skb;

	skb = mt76_connac_mcu_alloc_sta_req(&dev->mt76, &mvif->mt76, NULL);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	if (enable)
		mt76_connac_mcu_bss_omac_tlv(skb, vif);
	mt76_connac_mcu_bss_basic_tlv(skb, vif, sta, phy->mt76,
				      mvif->sta.wcid.idx, enable);

	if (enable && mvif->mt76.omac_idx >= EXT_BSSID_START &&
	    mvif->mt76.omac_idx < REPEATER_BSSID_START)
		mt76_connac_mcu_bss_ext_tlv(skb, &mvif->mt76);

	return mt76_mcu_skb_send_msg(&dev->mt76, skb,
				     MCU_EXT_CMD(BSS_INFO_UPDATE), true);
}

/* gen4m CMD_UPDATE_STA_RECORD (CID 0x13), byte-for-byte firmware ABI.
 * The vendor structure is naturally aligned to 136 bytes.  Keep the two
 * alignment bytes explicit so this layout is independent of host compiler
 * padding decisions.
 */
struct mt7663s_w103d_vendor_sta_record {
	u8 sta_idx;
	u8 sta_type;
	u8 mac_addr[ETH_ALEN];
	__le16 assoc_id;
	__le16 listen_interval;
	u8 bss_idx;
	u8 desired_phy_type;
	__le16 desired_non_ht_rates;
	__le16 basic_rates;
	u8 qos;
	u8 uapsd;
	u8 sta_state;
	u8 mcs_set;
	u8 sup_mcs32;
	u8 rsv1;
	u8 rx_mcs_bitmask[10];
	__le16 rx_highest_rate;
	__le32 tx_rate_info;
	__le16 ht_cap;
	__le16 ht_ext_cap;
	__le32 tx_bf_cap;
	u8 ampdu_param;
	u8 asel_cap;
	u8 rcpi;
	u8 need_resp;
	u8 uapsd_ac;
	u8 uapsd_sp;
	u8 wlan_idx;
	u8 bmc_wlan_idx;
	__le32 vht_cap;
	__le16 vht_rx_mcs_map;
	__le16 vht_rx_highest_rate;
	__le16 vht_tx_mcs_map;
	__le16 vht_tx_highest_rate;
	u8 rts_policy;
	u8 vht_opmode;
	u8 traffic_data_type;
	u8 tx_gf_mode;
	u8 tx_sgi_mode;
	u8 tx_stbc_mode;
	__le16 hw_default_fixed_rate;
	u8 tx_ampdu;
	u8 rx_ampdu;
	u8 align2[2];
	__le32 fixed_phy_rate;
	__le16 max_link_speed;
	__le16 min_link_speed;
	__le32 flags;
	u8 tx_ba_size;
	u8 rx_ba_size;
	u8 rsv3[2];
	u8 tx_bf_pfmu_info[32];
	u8 tx_amsdu_in_ampdu;
	u8 rx_amsdu_in_ampdu;
	u8 rsv5[2];
	__le32 tx_max_amsdu_in_ampdu_len;
} __packed;

struct mt7663s_w103d_vendor_sta_remove {
	u8 action_type;
	u8 sta_idx;
	u8 bss_idx;
	u8 rsv;
} __packed;

static_assert(sizeof(struct mt7663s_w103d_vendor_sta_record) == 136);
static_assert(sizeof(struct mt7663s_w103d_vendor_sta_remove) == 4);

static u16 mt7663s_w103d_vendor_rate_set(u32 rates, enum nl80211_band band)
{
	if (band == NL80211_BAND_2GHZ)
		return (rates & 0xf) | ((rates & 0xff0) << 2);

	return (rates & 0xff) << 6;
}

static u8 mt7663s_w103d_vendor_phy_type(struct ieee80211_sta *sta,
					       enum nl80211_band band)
{
	u8 phy_type;

	/* gen4m PHY_TYPE_BIT_HR_DSSS/ERP/OFDM are bits 0/1/3. */
	phy_type = band == NL80211_BAND_2GHZ ? BIT(0) | BIT(1) : BIT(3);
	if (sta->deflink.ht_cap.ht_supported)
		phy_type |= BIT(4);
	if (sta->deflink.vht_cap.vht_supported)
		phy_type |= BIT(5);

	return phy_type;
}

static u8 mt7663s_w103d_vendor_vht_opmode(struct ieee80211_sta *sta)
{
	u8 width;

	switch (sta->deflink.bandwidth) {
	case IEEE80211_STA_RX_BW_40:
		width = 1;
		break;
	case IEEE80211_STA_RX_BW_80:
		width = 2;
		break;
	case IEEE80211_STA_RX_BW_160:
		width = 3;
		break;
	default:
		width = 0;
		break;
	}

	return width | ((max_t(u8, sta->deflink.rx_nss, 1) - 1) << 4);
}

static int
mt7663s_w103d_vendor_activate_sta(struct mt7615_dev *dev,
					 struct mt7615_phy *phy,
					 struct ieee80211_vif *vif,
					 struct ieee80211_sta *sta)
{
	struct mt7615_vif *mvif = (struct mt7615_vif *)vif->drv_priv;
	struct mt7615_sta *msta = (struct mt7615_sta *)sta->drv_priv;
	struct cfg80211_chan_def *chandef = mvif->mt76.ctx ?
		&mvif->mt76.ctx->def : &phy->mt76->chandef;
	const struct ieee80211_sta_ht_cap *ht = &sta->deflink.ht_cap;
	const struct ieee80211_sta_vht_cap *vht = &sta->deflink.vht_cap;
	enum nl80211_band band = chandef->chan->band;
	u16 rates = mt7663s_w103d_vendor_rate_set(
		sta->deflink.supp_rates[band], band);
	u16 basic_rates = mt7663s_w103d_vendor_rate_set(
		vif->bss_conf.basic_rates, band);
	u8 uapsd_queues = sta->uapsd_queues & 0xf;
	struct mt7663s_w103d_vendor_sta_record req = {
		.sta_idx = msta->wcid.idx,
		/* STA_TYPE_LEGACY_AP = BIT(0) | BIT(6). */
		.sta_type = BIT(0) | BIT(6),
		.assoc_id = cpu_to_le16(sta->aid),
		.bss_idx = mvif->mt76.idx,
		.desired_phy_type = mt7663s_w103d_vendor_phy_type(sta, band),
		.desired_non_ht_rates = cpu_to_le16(rates),
		.basic_rates = cpu_to_le16(basic_rates),
		.qos = sta->wme,
		.uapsd = !!uapsd_queues,
		/* gen4m STA_STATE_3 has the wire value 2. */
		.sta_state = 2,
		.mcs_set = ht->mcs.rx_mask[0],
		.sup_mcs32 = !!(ht->mcs.rx_mask[4] & BIT(0)),
		.ht_cap = cpu_to_le16(ht->cap),
		.ampdu_param =
			FIELD_PREP(IEEE80211_HT_AMPDU_PARM_FACTOR,
				   ht->ampdu_factor) |
			FIELD_PREP(IEEE80211_HT_AMPDU_PARM_DENSITY,
				   ht->ampdu_density),
		.need_resp = 1,
		.uapsd_ac = uapsd_queues | (uapsd_queues << 4),
		.uapsd_sp = sta->max_sp,
		.wlan_idx = msta->wcid.idx,
		.bmc_wlan_idx = 0xff,
		.vht_cap = cpu_to_le32(vht->cap),
		.vht_rx_mcs_map = vht->vht_mcs.rx_mcs_map,
		.vht_rx_highest_rate = vht->vht_mcs.rx_highest,
		.vht_tx_mcs_map = vht->vht_mcs.tx_mcs_map,
		.vht_tx_highest_rate = vht->vht_mcs.tx_highest,
		/* RTS_POLICY_LEGACY in the vendor ABI. */
		.rts_policy = 3,
		.vht_opmode = mt7663s_w103d_vendor_vht_opmode(sta),
		.tx_ampdu = 1,
		.rx_ampdu = 1,
		.tx_ba_size = 64,
		.rx_ba_size = 64,
		.tx_amsdu_in_ampdu = vht->vht_supported,
		.rx_amsdu_in_ampdu = vht->vht_supported,
		.tx_max_amsdu_in_ampdu_len = cpu_to_le32(4096),
	};
	int ret;

	memcpy(req.mac_addr, sta->addr, ETH_ALEN);
	memcpy(req.rx_mcs_bitmask, ht->mcs.rx_mask,
	       sizeof(req.rx_mcs_bitmask));

	ret = mt76_mcu_send_msg(&dev->mt76,
		MCU_CMD_VENDOR_SET(MCU_CMD_VENDOR_UPDATE_STA_RECORD),
		&req, sizeof(req), true);
	dev_info(dev->mt76.dev,
		 "W103D: vendor STA_STATE_3 idx=%u bss=%u wlan=%u ampdu=1/1 result=%d\n",
		 req.sta_idx, req.bss_idx, req.wlan_idx, ret);

	return ret;
}

static int mt7663s_w103d_vendor_remove_sta(struct mt7615_dev *dev,
					   struct ieee80211_vif *vif,
					   struct ieee80211_sta *sta)
{
	struct mt7615_vif *mvif = (struct mt7615_vif *)vif->drv_priv;
	struct mt7615_sta *msta = (struct mt7615_sta *)sta->drv_priv;
	struct mt7663s_w103d_vendor_sta_remove req = {
		/* STA_REC_CMD_ACTION_STA */
		.action_type = 0,
		.sta_idx = msta->wcid.idx,
		.bss_idx = mvif->mt76.idx,
	};

	return mt76_mcu_send_msg(&dev->mt76,
		MCU_CMD_VENDOR_SET(MCU_CMD_VENDOR_REMOVE_STA_RECORD),
		&req, sizeof(req), false);
}

static int mt7663s_w103d_sta_add(struct mt7615_phy *phy,
					 struct ieee80211_vif *vif,
					 struct ieee80211_sta *sta, bool enable)
{
	struct mt7615_vif *mvif = (struct mt7615_vif *)vif->drv_priv;
	struct mt7615_dev *dev = phy->dev;
	struct mt76_sta_cmd_info info = {
		.sta = sta,
		.vif = vif,
		.offload_fw = false,
		.enable = enable,
		.newly = true,
		.cmd = MCU_EXT_CMD(STA_REC_UPDATE),
	};
	int ret;

	info.wcid = sta ? (struct mt76_wcid *)sta->drv_priv :
		&mvif->sta.wcid;
	ret = mt76_connac_mcu_sta_cmd(phy->mt76, &info);
	if (ret || !sta)
		return ret;

	if (enable)
		return mt7663s_w103d_vendor_activate_sta(dev, phy, vif, sta);

	return mt7663s_w103d_vendor_remove_sta(dev, vif, sta);
}

static int mt7663s_w103d_sta_ba(struct mt7615_dev *dev,
					struct ieee80211_ampdu_params *params,
					bool enable, bool tx)
{
	struct mt7615_sta *msta = (struct mt7615_sta *)params->sta->drv_priv;
	struct wtbl_req_hdr *wtbl_hdr;
	struct tlv *sta_wtbl;
	struct sk_buff *skb;
	int ret;

	/* CONNAC1/MT7663 requires the STA_REC_BA and nested WTBL_BA TLVs in
	 * one STA_REC_UPDATE transaction.  The generic helper emits two
	 * independent commands; that is suitable for newer firmware, but the
	 * 2021 MT7663 N9 silently accepts them without activating the BA entry.
	 * Keep the original mt7615 legacy wire format in this isolated module.
	 */
	skb = mt76_connac_mcu_alloc_sta_req(&dev->mt76, &msta->vif->mt76,
					    &msta->wcid);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	mt76_connac_mcu_sta_ba_tlv(skb, params, enable, tx);
	sta_wtbl = mt76_connac_mcu_add_tlv(skb, STA_REC_WTBL,
					   sizeof(struct tlv));
	wtbl_hdr = mt76_connac_mcu_alloc_wtbl_req(&dev->mt76, &msta->wcid,
						  WTBL_SET, sta_wtbl, &skb);
	if (IS_ERR(wtbl_hdr))
		return PTR_ERR(wtbl_hdr);

	mt76_connac_mcu_wtbl_ba_tlv(&dev->mt76, skb, params, enable, tx,
				    sta_wtbl, wtbl_hdr);
	ret = mt76_mcu_skb_send_msg(&dev->mt76, skb,
				    MCU_EXT_CMD(STA_REC_UPDATE), true);
	dev_info_ratelimited(dev->mt76.dev,
		"W103D: %s BA %s tid=%u win=%u result=%d\n",
		tx ? "TX" : "RX", enable ? "start" : "stop",
		params->tid, params->buf_size, ret);

	return ret;
}

static int mt7663s_w103d_sta_tx_ba(struct mt7615_dev *dev,
					   struct ieee80211_ampdu_params *params,
					   bool enable)
{
	return mt7663s_w103d_sta_ba(dev, params, enable, true);
}

static int mt7663s_w103d_sta_rx_ba(struct mt7615_dev *dev,
					   struct ieee80211_ampdu_params *params,
					   bool enable)
{
	return mt7663s_w103d_sta_ba(dev, params, enable, false);
}

static int mt7663s_w103d_sta_update_hdr_trans(struct mt7615_dev *dev,
						 struct ieee80211_vif *vif,
						 struct ieee80211_sta *sta)
{
	struct mt7615_sta *msta = (struct mt7615_sta *)sta->drv_priv;

	return mt76_connac_mcu_sta_update_hdr_trans(&dev->mt76, vif,
						    &msta->wcid,
						    MCU_EXT_CMD(STA_REC_UPDATE));
}

static int mt7663s_w103d_set_pm_state(struct mt7615_dev *dev, int band,
					      int state)
{
	return mt76_connac_mcu_set_pm(&dev->mt76, band, state);
}

void mt7663s_w103d_install_legacy_ops(struct mt7615_mcu_ops *ops)
{
	ops->set_pm_state = mt7663s_w103d_set_pm_state;
	ops->add_dev_info = mt7663s_w103d_add_dev_info;
	ops->add_bss_info = mt7663s_w103d_add_bss_info;
	ops->add_tx_ba = mt7663s_w103d_sta_tx_ba;
	ops->add_rx_ba = mt7663s_w103d_sta_rx_ba;
	ops->sta_add = mt7663s_w103d_sta_add;
	ops->set_sta_decap_offload = mt7663s_w103d_sta_update_hdr_trans;
}

void mt7663s_w103d_configure_filter(struct ieee80211_hw *hw,
				    unsigned int changed_flags,
				    unsigned int *total_flags,
				    u64 multicast)
{
	struct mt7615_dev *dev = mt7615_hw_dev(hw);
	struct mt7615_phy *phy = mt7615_hw_phy(hw);
	unsigned int supported = FIF_ALLMULTI | FIF_OTHER_BSS |
		FIF_FCSFAIL | FIF_CONTROL | FIF_BCN_PRBRESP_PROMISC |
		FIF_PROBE_REQ;
	u32 rfcr = 0;
	u32 rfcr1 = MT_WF_RFCR1_DROP_ACK |
		MT_WF_RFCR1_DROP_BF_POLL |
		MT_WF_RFCR1_DROP_CFEND |
		MT_WF_RFCR1_DROP_CFACK;

	/* V64 used the normal host RFCR path.  Do not send SET_RX_FILTER here:
	 * its NDIS-style data packet mask excludes management traffic on this
	 * firmware, leaving only FCS-failed frames visible during a scan.
	 * Keep the host filter deliberately small and deterministic: all valid
	 * frames pass, while corrupt frames are dropped unless explicitly asked
	 * for by monitor/test mode.
	 */
	*total_flags &= supported;
	if (!(*total_flags & FIF_FCSFAIL))
		rfcr |= MT_WF_RFCR_DROP_FCSFAIL;
	phy->rxfilter = rfcr;

	/* RFCR1_DROP_BA also suppresses Block Ack action traffic on the MT7663
	 * N9 path, not only control BA frames.  Leaving the firmware default set
	 * makes every successful ADDBA response disappear before SDIO.  Preserve
	 * the other low-value control filters but explicitly admit BA traffic.
	 */
	mt7663s_w103d_reg_wr(&dev->mt76, 0x820f5000, rfcr);
	mt7663s_w103d_reg_wr(&dev->mt76, 0x820f5004, rfcr1);
}

void mt7663s_w103d_note_assoc(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif, u64 changed)
{
	struct mt7615_dev *dev = mt7615_hw_dev(hw);
	struct mt76_phy *mphy = hw->priv;
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(&dev->mt76);

	if (state && (changed & BSS_CHANGED_ASSOC) && vif->cfg.assoc &&
	    mphy->chandef.chan &&
	    mphy->chandef.chan->band == NL80211_BAND_5GHZ)
		WRITE_ONCE(state->join_privilege_held, true);
}

static int mt7663s_w103d_channel_privilege(struct mt7615_dev *dev,
					   struct cfg80211_chan_def *chandef,
					   bool request);

int mt7663s_w103d_hw_scan(struct ieee80211_hw *hw,
			  struct ieee80211_vif *vif,
			  struct ieee80211_scan_request *req)
{
	struct mt7615_dev *dev = mt7615_hw_dev(hw);
	struct mt76_phy *mphy = hw->priv;
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(&dev->mt76);
	int ret;

	if (!mt7615_firmware_offload(dev))
		return 1;

	/* Association holds a JOIN privilege on 5 GHz.  The firmware does not
	 * expire it reliably after deauthentication, and its scan engine then
	 * cannot acquire any channel.  Release it before every new scan.
	 */
	if (state && xchg(&state->join_privilege_held, false)) {
		mt7663s_w103d_channel_privilege(dev, &mphy->chandef, false);
		msleep(50);
	}

	/* A firmware CANCEL after a result is received leaves the 2021 N9 scan
	 * engine unable to deliver beacons on subsequent requests.  All active
	 * requests are converted to bounded passive scans below, so let them
	 * finish naturally and never pre-cancel an idle engine here.
	 */
	mt7615_mutex_acquire(dev);
	ret = mt76_connac_mcu_hw_scan(mphy, vif, req);
	mt7615_mutex_release(dev);

	return ret;
}

void mt7663s_w103d_cancel_hw_scan(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif)
{
	struct mt76_phy *mphy = hw->priv;

	/* Complete only the host-side request.  Sending MCU_CE_CMD(CANCEL_HW_SCAN)
	 * poisons the next scan on this firmware; its bounded passive scan can
	 * safely complete in the background.
	 */
	if (test_and_clear_bit(MT76_HW_SCANNING, &mphy->state)) {
		struct cfg80211_scan_info info = {
			.aborted = true,
		};

		ieee80211_scan_completed(hw, &info);
	}
}

int mt7663s_w103d_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			  struct ieee80211_vif *vif,
			  struct ieee80211_sta *sta,
			  struct ieee80211_key_conf *key)
{
	/* The inherited mt7615 set_key path programs WTBL through direct MAC
	 * register accesses.  W103D deliberately blocks those accesses because
	 * this MT7663 firmware does not service the internal 0x82xxxxxx mailbox
	 * range reliably.  Claiming that key installation succeeded therefore
	 * leaves data frames encrypted with a key that never reached hardware:
	 * EAPOL completes, while ARP and DHCP disappear.
	 *
	 * Let mac80211 perform encryption and decryption in software.  No key was
	 * ever installed in WTBL, so removing one is already complete.
	 */
	if (cmd == DISABLE_KEY)
		return 0;

	return -EOPNOTSUPP;
}

void mt7663s_w103d_limit_vht80(struct mt7615_dev *dev)
{
	struct ieee80211_supported_band *sband =
		dev->mt76.hw->wiphy->bands[NL80211_BAND_5GHZ];
	struct ieee80211_sta_vht_cap *vht_cap;

	if (!sband)
		return;
	vht_cap = &sband->vht_cap;

	/* MT7663 supports VHT20/40/80 only.  mt7615_init_device() enables
	 * 160/80+80 for a non-DBDC radio unconditionally, which lets mac80211
	 * select an unusable 160 MHz chandef when the AP advertises one.  The
	 * firmware then receives a 20/40 MHz legacy privilege request because
	 * that command has no 160 MHz encoding in this transport.  RX beacons
	 * still arrive on the primary channel, but every data TX misses its ACK.
	 */
	vht_cap->cap &= ~(IEEE80211_VHT_CAP_SHORT_GI_160 |
			  IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK);

	dev_info(dev->mt76.dev,
		 "W103D: limited MT7663S channel width to VHT80 (cap=0x%08x)\n",
		 vht_cap->cap);
}

struct mt7663s_w103d_ch_privilege {
	u8 bss_idx;
	u8 token_id;
	u8 action;
	u8 primary_chan;
	u8 rf_sco;
	u8 rf_band;
	u8 rf_width;
	u8 center_seg1;
	u8 center_seg2;
	u8 req_type;
	u8 dbdc_band;
	u8 rsv0;
	__le32 max_interval;
	u8 rsv1[8];
} __packed;

static int mt7663s_w103d_channel_privilege(struct mt7615_dev *dev,
						 struct cfg80211_chan_def *chandef,
						 bool request)
{
	struct mt7663s_w103d_ch_privilege privilege = {
		.token_id = 1,
		.action = request ? 0 : 1,
		.dbdc_band = 4,
	};

	if (request) {
		privilege.primary_chan = chandef->chan->hw_value;
		if (chandef->width == NL80211_CHAN_WIDTH_40)
			privilege.rf_sco = chandef->center_freq1 >
					   chandef->chan->center_freq ? 1 : 3;
		privilege.rf_band = 2;
		privilege.rf_width = chandef->width == NL80211_CHAN_WIDTH_80;
		privilege.center_seg1 = ieee80211_frequency_to_channel(
			chandef->center_freq1);
		privilege.max_interval = cpu_to_le32(6000);
	}

	return mt76_mcu_send_msg(&dev->mt76,
				 MCU_CMD(VENDOR_CH_PRIVILEGE), &privilege,
				 sizeof(privilege), false);
}

int mt7663s_w103d_set_channel(struct mt76_phy *mphy)
{
	struct mt7615_phy *phy = mphy->priv;
	struct mt7615_dev *dev = phy->dev;
	struct cfg80211_chan_def *chandef = &mphy->chandef;
	bool use_privilege;
	int ret;

	if (!mt7663s_w103d_active() || !chandef->chan)
		return mt7615_set_channel(mphy);

	use_privilege = chandef->chan->band == NL80211_BAND_5GHZ &&
			!mphy->offchannel;
	if (use_privilege)
		mt7663s_w103d_channel_privilege(dev, chandef, false);

	ret = mt7615_set_channel(mphy);
	if (ret)
		return ret;

	/* MT_CHFREQ lives in the intentionally blocked internal RMAC range.
	 * Keep the legacy RX descriptor validation token aligned with the
	 * channel selected by mac80211 without reopening that mailbox read.
	 */
	phy->chfreq = chandef->chan->hw_value;
	if (!use_privilege)
		return 0;

	ret = mt7663s_w103d_channel_privilege(dev, chandef, true);
	if (!ret)
		msleep(100);

	return ret;
}

/* Determine whether a command should wait for a firmware response.
 * For v3 firmware on SDIO, the vendor driver uses legacy sta_update_ops
 * which do not generate responses for UNI SET commands.  This function
 * mirrors the vendor gen4m mt7615_mcu_wait_resp() decision logic.
 */
bool mt7663s_w103d_should_wait_resp(int cmd)
{
	if (cmd & __MCU_CMD_FIELD_QUERY)
		return true;

	if (cmd & __MCU_CMD_FIELD_UNI)
		return false;

	return !(FIELD_GET(__MCU_CMD_FIELD_EXT_ID, cmd) &&
		 FIELD_GET(__MCU_CMD_FIELD_EXT_ID, cmd) !=
		 MCU_EXT_CMD_EFUSE_BUFFER_MODE);
}

int mt7663s_w103d_mcu_send_msg(struct mt76_dev *mdev, int cmd,
				      const void *data, int len, bool wait_resp)
{
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	struct {
		u8 bss_idx;
		u8 ps_profile;
		u8 reserved[2];
	} ps_req;
	struct sk_buff *skb;
	int ret;

	/* The MT7663 v3 firmware follows the vendor gen4m four-byte
	 * CMD_PS_PROFILE ABI.  mt76's common helper uses a compact two-byte
	 * request, which leaves this firmware in its default DTIM power-save
	 * mode even when mac80211 requests CAM (power_save off).
	 */
	if (cmd == MCU_CE_CMD(SET_PS_PROFILE) && len == 2) {
		memset(&ps_req, 0, sizeof(ps_req));
		memcpy(&ps_req, data, len);
		data = &ps_req;
		len = sizeof(ps_req);
	}

	skb = mt76_mcu_msg_alloc(mdev, data, len);
	if (!skb)
		return -ENOMEM;

	if (mt7663s_w103d_cmd_no_ack(cmd))
		wait_resp = false;

	if (state)
		state->suppress_noack_completion = true;
	ret = mt76_mcu_skb_send_msg(mdev, skb, cmd, wait_resp);
	if (state)
		state->suppress_noack_completion = false;

	return ret;
}

/* ========================================================================
 * V59 port: Firmware download headroom limit for Meson G12A.
 *
 * Limit a W103D firmware frame to two SDIO blocks.  This stays below the
 * Meson G12A controller's three-block SRAM request limit and leaves room
 * for the MT7663 MCU header plus the four-byte SDIO terminator.
 * ======================================================================== */

unsigned int mt7663s_w103d_fw_headroom(struct mt76_dev *mdev)
{
	u32 block_size = mdev->sdio.func->cur_blksize;
	u32 frame_len = min_t(u32, 2 * block_size, mdev->sdio.xmit_buf_sz);
	u32 overhead = sizeof(struct mt7615_mcu_txd) + sizeof(u32);
	u32 payload_len;

	if (!block_size || frame_len <= overhead)
		return sizeof(struct mt7615_mcu_txd);

	payload_len = frame_len - overhead;
	return 4096 - payload_len;
}

/* ========================================================================
 * V60 port: TX size limit — prevent SDIO TX deadlock.
 *
 * Reject skbs whose TXD + 4-byte terminator exceed the SDIO transmit
 * buffer size.  This prevents the Amlogic SDIO controller from stalling
 * when an oversized frame is written.
 * ======================================================================== */

bool mt7663s_w103d_check_tx_size(struct mt76_dev *mdev, struct sk_buff *skb)
{
	u32 total = skb->len + sizeof(u32);

	if (total > mdev->sdio.xmit_buf_sz) {
		dev_err_ratelimited(mdev->dev,
			"W103D: TX frame too large (%u > %u), dropping\n",
			total, mdev->sdio.xmit_buf_sz);
		return false;
	}
	return true;
}

static u16
mt7663s_w103d_tx_rate_val(struct mt76_phy *mphy,
			  const struct ieee80211_tx_rate *rate, bool stbc,
			  u8 *bw)
{
	u8 phy, nss, rate_idx;
	u16 rateval = 0;

	*bw = 0;
	if (rate->flags & IEEE80211_TX_RC_VHT_MCS) {
		rate_idx = ieee80211_rate_get_vht_mcs(rate);
		nss = ieee80211_rate_get_vht_nss(rate);
		phy = MT_PHY_TYPE_VHT;
		if (rate->flags & IEEE80211_TX_RC_40_MHZ_WIDTH)
			*bw = 1;
		else if (rate->flags & IEEE80211_TX_RC_80_MHZ_WIDTH)
			*bw = 2;
	} else if (rate->flags & IEEE80211_TX_RC_MCS) {
		rate_idx = rate->idx;
		nss = 1 + (rate->idx >> 3);
		phy = rate->flags & IEEE80211_TX_RC_GREEN_FIELD ?
			MT_PHY_TYPE_HT_GF : MT_PHY_TYPE_HT;
		if (rate->flags & IEEE80211_TX_RC_40_MHZ_WIDTH)
			*bw = 1;
	} else {
		const struct ieee80211_rate *legacy;
		u16 val;

		legacy = &mphy->hw->wiphy->bands[NL80211_BAND_5GHZ]
					->bitrates[rate->idx];
		val = rate->flags & IEEE80211_TX_RC_USE_SHORT_PREAMBLE ?
			legacy->hw_value_short : legacy->hw_value;
		nss = 1;
		phy = val >> 8;
		rate_idx = val & 0xff;
	}

	if (stbc && nss == 1) {
		nss++;
		rateval |= MT_TX_RATE_STBC;
	}

	return rateval | FIELD_PREP(MT_TX_RATE_IDX, rate_idx) |
		FIELD_PREP(MT_TX_RATE_MODE, phy) |
		FIELD_PREP(MT_TX_RATE_NSS, nss - 1);
}

static void
mt7663s_w103d_fix_5g_tx_rate(struct mt7615_dev *dev,
			     struct ieee80211_sta *sta,
			     struct mt76_tx_info *tx_info, bool is_data,
			     bool firmware_ba)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx_info->skb);
	struct ieee80211_sta_rates *rates;
	struct ieee80211_tx_rate rate = {};
	__le32 *txwi = (__le32 *)tx_info->skb->data;
	u16 rateval;
	u8 bw;

	if (!is_data) {
		/* Management/control traffic cannot fall back to the inaccessible
		 * WTBL rate table.  Index 0 is mandatory 6 Mbps OFDM on 5 GHz.
		 */
		rate.idx = 0;
		rate.count = 8;
		rate.flags = 0;
	} else if ((info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE) &&
	    info->control.rates[0].idx >= 0) {
		rate = info->control.rates[0];
	} else {
		rcu_read_lock();
		rates = rcu_dereference(sta->rates);
		if (rates) {
			rate.idx = rates->rate[0].idx;
			rate.count = rates->rate[0].count;
			rate.flags = rates->rate[0].flags;
		}
		rcu_read_unlock();
	}

	/* Before minstrel publishes a table, use mandatory 6 Mbps OFDM. */
	if (rate.idx < 0 || !rate.count) {
		rate.idx = 0;
		rate.count = 8;
		rate.flags = 0;
	}

	rateval = mt7663s_w103d_tx_rate_val(&dev->mphy, &rate,
					    info->flags & IEEE80211_TX_CTL_STBC,
					    &bw);
	txwi[2] |= cpu_to_le32(MT_TXD2_FIX_RATE);
	/* mac80211 does not mark packets as AMPDU when the V3 firmware owns BA,
	 * so the shared TXWI writer sets BA_DISABLE.  Clear it only for unicast
	 * QoS data; firmware will aggregate these packets after its BA agreement
	 * is live.  Management, multicast and non-QoS traffic remains no-BA.
	 */
	if (firmware_ba)
		txwi[2] &= cpu_to_le32(~MT_TXD2_BA_DISABLE);
	if (!(rate.flags & (IEEE80211_TX_RC_MCS | IEEE80211_TX_RC_VHT_MCS)))
		txwi[2] |= cpu_to_le32(MT_TXD2_BA_DISABLE);
	txwi[3] &= cpu_to_le32(~MT_TXD3_REM_TX_COUNT);
	txwi[3] |= cpu_to_le32(FIELD_PREP(MT_TXD3_REM_TX_COUNT,
					   clamp_t(u8, rate.count, 1, 8)));
	txwi[6] = cpu_to_le32(MT_TXD6_FIXED_BW |
				 FIELD_PREP(MT_TXD6_BW, bw) |
				 FIELD_PREP(MT_TXD6_TX_RATE, rateval));
	if (rate.flags & IEEE80211_TX_RC_SHORT_GI)
		txwi[6] |= cpu_to_le32(MT_TXD6_SGI);
	if (info->flags & IEEE80211_TX_CTL_LDPC)
		txwi[6] |= cpu_to_le32(MT_TXD6_LDPC);
}

int mt7663s_w103d_tx_prepare_skb(struct mt76_dev *mdev, void *txwi_ptr,
					 enum mt76_txq_id qid, struct mt76_wcid *wcid,
					 struct ieee80211_sta *sta,
					 struct mt76_tx_info *tx_info)
{
	struct mt7615_dev *dev = container_of(mdev, struct mt7615_dev, mt76);
	struct ieee80211_hdr *hdr = (void *)tx_info->skb->data;
	bool is_data = ieee80211_is_data(hdr->frame_control);
	bool firmware_ba = ieee80211_is_data_qos(hdr->frame_control) &&
		!is_multicast_ether_addr(hdr->addr1);
	u32 sdio_len;
	int ret;

	if (mt7663s_w103d_active()) {
		sdio_len = round_up(tx_info->skb->len + MT_USB_TXD_SIZE, 4) + 4;
		if (sdio_len > mdev->sdio.xmit_buf_sz) {
			dev_err_ratelimited(mdev->dev,
				"W103D: oversized data skb rejected before queueing: %u > %u\n",
				sdio_len, mdev->sdio.xmit_buf_sz);
			ieee80211_free_txskb(mdev->hw, tx_info->skb);
			return -EMSGSIZE;
		}
	}

	ret = mt7663_usb_sdio_tx_prepare_skb(mdev, txwi_ptr, qid, wcid, sta,
					     tx_info);
	if (ret || !sta || !dev->mphy.chandef.chan ||
	    dev->mphy.chandef.chan->band != NL80211_BAND_5GHZ)
		return ret;

	/* The shared SDIO rate worker writes the WTBL rate table through blocked
	 * 0x82xxxx registers, then marks that table valid.  Carry minstrel's
	 * selected rate in each TX descriptor instead, bypassing that inaccessible
	 * table while retaining VHT bandwidth, NSS and rate adaptation.  Force
	 * management/control frames to mandatory 6 Mbps OFDM as well: leaving
	 * their descriptor unfixed still selects the same inaccessible WTBL table.
	 */
	mt7663s_w103d_fix_5g_tx_rate(dev, sta, tx_info, is_data, firmware_ba);

	return 0;
}

/* ========================================================================
 * MCU synchronous write with alignment.
 * ======================================================================== */

static int mt7663s_w103d_sdio_write(struct mt76_dev *mdev,
				    struct sk_buff *skb, bool block_align)
{
	struct mt76_sdio *sdio = &mdev->sdio;
	u32 block_size = sdio->func->cur_blksize;
	int old_len, tx_len, err;

	/* V60: reject oversized frames before adding terminator. */
	if (!mt7663s_w103d_check_tx_size(mdev, skb))
		return -EMSGSIZE;

	__skb_put_zero(skb, sizeof(u32));
	old_len = skb->len;

	/* Firmware download uses complete SDIO blocks.  Once the MCU is running,
	 * match the stock aggregation path and transfer only a dword-aligned
	 * frame; padding small runtime commands to 512 bytes changes their HIF
	 * framing on MT7663.
	 */
	if (block_align && old_len > block_size &&
	    roundup(old_len, block_size) <= sdio->xmit_buf_sz)
		tx_len = roundup(old_len, block_size);
	else if (test_bit(MT76_STATE_MCU_RUNNING, &mdev->phy.state))
		tx_len = roundup(old_len, sizeof(u32));
	else
		tx_len = roundup(old_len, block_size);

	err = __skb_grow(skb, tx_len);
	if (err)
		return err;
	memset(skb->data + old_len, 0, tx_len - old_len);

	sdio_claim_host(sdio->func);
	err = sdio_writesb(sdio->func, MCR_WTDR1, skb->data, tx_len);
	sdio_release_host(sdio->func);
	if (!err) {
		if (tx_len >= 512)
			usleep_range(15000, 20000);
		else if (tx_len >= 128)
			usleep_range(3000, 5000);
		else
			usleep_range(1000, 2000);
	}

	if (err)
		dev_err(mdev->dev, "W103D synchronous SDIO MCU write failed: %d\n",
			err);

	return err;
}

void mt7663s_w103d_purge_stale_responses(struct mt76_dev *mdev)
{
	struct sk_buff_head *res_q = &mdev->mcu.res_q;
	struct sk_buff *skb;
	int count = 0;

	spin_lock_bh(&res_q->lock);
	while ((skb = __skb_dequeue(res_q)) != NULL) {
		dev_kfree_skb(skb);
		count++;
	}
	spin_unlock_bh(&res_q->lock);

	if (count)
		dev_warn(mdev->dev,
			 "W103D: purged %d stale MCU response(s)\n", count);
}

/* ========================================================================
 * MCU message send (main entry point for MCU commands).
 * ======================================================================== */

int mt7663s_w103d_mcu_send_message(struct mt76_dev *mdev,
				   struct sk_buff *skb, int cmd, int *seq)
{
	struct mt7615_dev *dev = container_of(mdev, struct mt7615_dev, mt76);
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	int err;

	if (cmd == MCU_EXT_CMD(EDCA_UPDATE) ||
	    cmd == MCU_EXT_CMD(DBDC_CTRL) ||
	    cmd == 0x0004005d) {
		dev_kfree_skb(skb);
		return 0;
	}

	if (state && state->eeprom_settle_pending) {
		/* Let the outer MCU wait consume EFUSE_BUFFER_MODE first. */
		msleep(1600);
		mt7663s_w103d_purge_stale_responses(mdev);
		state->eeprom_settle_pending = false;
		dev_info(mdev->dev,
			 "W103D: EEPROM settled, stale MCU responses purged\n");
	}

	if (cmd == MCU_EXT_CMD(EFUSE_BUFFER_MODE))
		mt7663s_w103d_select_eeprom_source(mdev, skb);

	mt7615_mcu_fill_msg(dev, skb, cmd, seq);
	if (cmd == MCU_CE_CMD(START_HW_SCAN)) {
		struct mt7615_mcu_txd *txd = (void *)skb->data;
		struct mt76_connac_hw_scan_req *req = (void *)(txd + 1);
		struct sdio_func *func = mdev->sdio.func;
		u32 whier;
		u16 channels;

		/* The 2021 W103D N9 stops on channel 1 for a directed active
		 * scan (the form used by wpa_supplicant/NetworkManager).  A
		 * wildcard passive request traverses the complete channel list
		 * and returns the same broadcast BSS entries reliably.  Keep the
		 * caller's channel list and random address, but remove directed
		 * SSIDs/probes and use the normal passive dwell time.
		 */
		if (skb->len >= sizeof(*txd) + sizeof(*req) && req->scan_type) {
			channels = req->channels_num + req->ext_channels_num;
			req->scan_type = 0;
			req->ssid_type = BIT(0);
			req->ssids_num = 0;
			req->probe_req_num = 0;
			req->ssid_type_ext = 0;
			req->channel_dwell_time = cpu_to_le16(120);
			req->channel_min_dwell_time = cpu_to_le16(120);
			req->timeout_value = cpu_to_le16(channels * 120);
			dev_info_ratelimited(mdev->dev,
				"W103D: active scan converted to passive (%u channels)\n",
				channels);
		}

		/* MT7663 sends scan results and SCAN_DONE through the MCU RX queue. */
		txd->s2d_index = MCU_S2D_H2CN;
		sdio_claim_host(func);
		whier = sdio_readl(func, MCR_WHIER, NULL);
		sdio_writel(func, whier | WHIER_RX1_DONE_INT_EN,
			    MCR_WHIER, NULL);
		sdio_release_host(func);
	}

	if (mt7663s_w103d_cmd_no_ack(cmd)) {
		if (cmd & __MCU_CMD_FIELD_UNI) {
			struct mt7615_uni_txd *uni = (void *)skb->data;

			uni->option &= ~MCU_CMD_ACK;
		} else {
			struct mt7615_mcu_txd *txd = (void *)skb->data;

			/* Do not ask the firmware to enqueue a response that the caller
			 * deliberately will not consume.  A pending MCU response can share
			 * a read-clear WHISR sample with the register-mailbox ACK, losing the
			 * RX0 notification and leaving the SDIO function interrupt asserted.
			 */
			txd->ext_cid_ack = 0;
		}
	}

	if (cmd == MCU_CMD(FW_SCATTER)) {
		struct mt7615_mcu_txd *txd = (void *)skb->data;

		txd->seq = 0;
		if (seq)
			*seq = 0;
	}

	/*
	 * The W103D host never completes the first runtime command queued via
	 * the generic asynchronous MCU TX path.  Keep only MCU control traffic
	 * synchronous; ordinary mac80211 data frames still use the stock queue.
	 */
	err = mt7663s_w103d_sdio_write(mdev, skb,
				       cmd == MCU_CE_CMD(START_HW_SCAN));
	if (!err && cmd == MCU_EXT_CMD(EFUSE_BUFFER_MODE) && state)
		state->eeprom_settle_pending = true;
	if (!err && mt7663s_w103d_cmd_no_ack(cmd) && seq &&
	    (!state || !state->suppress_noack_completion))
		mt7663s_w103d_complete_no_ack(mdev, cmd, *seq);
	dev_kfree_skb(skb);
	return err;
}

/* ========================================================================
 * IRQ handler — CMD52 to avoid Meson GX DMA deadlock.
 * ======================================================================== */

void mt7663s_w103d_sdio_irq(struct sdio_func *func)
{
	struct mt7615_dev *dev = sdio_get_drvdata(func);
	struct mt76_dev *mdev = &dev->mt76;
	struct mt76_sdio *sdio = &mdev->sdio;
	struct mt7663s_w103d_intr_state *state =
		mt7663s_w103d_intr_state(mdev);
	int err;

	/* Clear interrupt via CMD52 (sdio_writeb) unconditionally first.
	 * This de-asserts DAT[1] immediately, preventing Meson GX SDIO DMA
	 * hardware deadlock and preventing level-triggered IRQ storms that would
	 * freeze CPU cores and block other peripherals (such as USB NICs).
	 */
	sdio_writeb(sdio->func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &err);
	if (err)
		return;

	if (!test_bit(MT76_STATE_INITIALIZED, &mdev->phy.state) ||
	    test_bit(MT76_MCU_RESET, &mdev->phy.state) ||
	    test_bit(MT76_STATE_SUSPEND, &mdev->phy.state))
		return;

	if (state && atomic_xchg(&state->irq_queued, 1))
		return;

	mt76_worker_schedule(&sdio->txrx_worker);
}

/* ========================================================================
 * RX check — fill missing CH_FREQ for scan responses.
 * ======================================================================== */

static u8 mt7663s_w103d_extract_frame_channel(const void *data, int len)
{
	const __le32 *rxd = data;
	u32 rxd0 = le32_to_cpu(rxd[0]);
	u32 rxd1 = le32_to_cpu(rxd[1]);
	int offset = 16;
	const u8 *p;
	u16 fc;
	int ie_start, ie_len;

	if (rxd0 & MT_RXD0_NORMAL_GROUP_4)
		offset += 16;
	if (rxd0 & MT_RXD0_NORMAL_GROUP_1)
		offset += 16;
	if (rxd0 & MT_RXD0_NORMAL_GROUP_2)
		offset += 8;
	if (rxd0 & MT_RXD0_NORMAL_GROUP_3)
		offset += 24;

	if (rxd1 & MT_RXD1_NORMAL_HDR_OFFSET)
		offset += 2;

	if (offset + 36 > len)
		return 0;

	p = (const u8 *)data + offset;
	fc = p[0] | ((u16)p[1] << 8);

	/* Check if Management frame (type 0) and Beacon (0x80) or ProbeResp (0x50) */
	if ((fc & 0x000c) != 0)
		return 0;
	if ((fc & 0x00f0) != 0x0080 && (fc & 0x00f0) != 0x0050)
		return 0;

	/* Fixed fields: 24 (hdr) + 12 (timestamp 8 + bcn_int 2 + capab 2) = 36 */
	ie_start = offset + 36;
	ie_len = len - ie_start;

	while (ie_len >= 2) {
		const u8 *ie = (const u8 *)data + ie_start;
		u8 tag = ie[0];
		u8 tlen = ie[1];

		if (2 + tlen > ie_len)
			break;

		/* Tag 3: DS Parameter Set (Channel 1..14) */
		if (tag == 3 && tlen == 1) {
			u8 ch = ie[2];
			if (ch >= 1 && ch <= 14)
				return ch;
		}

		/* Tag 61: HT Operation (Primary Channel 1..196) */
		if (tag == 61 && tlen >= 1) {
			u8 ch = ie[2];
			if (ch >= 1 && ch <= 196)
				return ch;
		}

		ie_start += 2 + tlen;
		ie_len -= 2 + tlen;
	}

	return 0;
}

bool mt7663s_w103d_rx_check(struct mt76_dev *mdev, void *data, int len)
{
	struct mt7615_dev *dev = container_of(mdev, struct mt7615_dev, mt76);
	__le32 *rxd = data;
	u32 rxd0, rxd1, type;

	if (len < 8)
		return false;

	rxd0 = le32_to_cpu(rxd[0]);
	rxd1 = le32_to_cpu(rxd[1]);
	type = FIELD_GET(MT_RXD0_PKT_TYPE, rxd0);

	if (mt7663s_w103d_active() && type == PKT_TYPE_TXRX_NOTIFY) {
		dev_warn_once(mdev->dev,
			      "W103D: ignoring SDIO TXRX_NOTIFY without tx_cleanup\n");
		return false;
	}

	/* MT7663 scan responses do not carry CH_FREQ in the normal RXD.
	 * Parse the beacon/probe response payload to extract the actual channel
	 * (from DS Parameter Set or HT Operation IEs) so that APs across all
	 * channels (e.g. Channel 6 or 5GHz) are not falsely dropped by mac80211.
	 * MT76_SCANNING covers mac80211 software scans when hw_scan is disabled.
	 */
	if (mt7663s_w103d_active() &&
	    type == PKT_TYPE_NORMAL &&
	    (test_bit(MT76_HW_SCANNING, &mdev->phy.state) ||
	     test_bit(MT76_SCANNING, &mdev->phy.state)) &&
	    !FIELD_GET(MT_RXD1_NORMAL_CH_FREQ, rxd1)) {
		u8 ch = mt7663s_w103d_extract_frame_channel(data, len);

		if (!ch)
			ch = dev->phy.chfreq ? dev->phy.chfreq : 1;

		rxd1 |= FIELD_PREP(MT_RXD1_NORMAL_CH_FREQ, ch);
		rxd[1] = cpu_to_le32(rxd1);
		dev_info_ratelimited(mdev->dev,
			"W103D: filled missing scan RX channel %u\n",
			ch);
	}

	return mt7615_rx_check(mdev, data, len);
}

/* The legacy V3 firmware consumes Block Ack action frames.  Instead of
 * forwarding RX ADDBA/DELBA to mac80211 it reports the negotiated session in
 * unsolicited legacy MCU events, as the vendor gen4m driver expects.  Teach
 * the isolated transport to translate those events into mt76 reorder state.
 */
#define W103D_MCU_EVENT_RX_ADDBA	0x0a
#define W103D_MCU_EVENT_RX_DELBA	0x0b

struct mt7663s_w103d_rx_addba_event {
	u8 wcid;
	u8 dialog_token;
	__le16 ba_param;
	__le16 timeout;
	__le16 start_seq;
} __packed;

struct mt7663s_w103d_rx_delba_event {
	u8 wcid;
	u8 tid;
	u8 reserved[2];
} __packed;

static bool
mt7663s_w103d_handle_rx_ba_event(struct mt76_dev *mdev, struct sk_buff *skb)
{
	struct mt7615_mcu_rxd *rxd;
	struct mt76_wcid *wcid;
	u8 wcid_idx, tid;
	int ret = 0;

	if (skb->len < sizeof(*rxd))
		return false;

	rxd = (struct mt7615_mcu_rxd *)skb->data;
	if (le32_get_bits(rxd->rxd[0], MT_RXD0_PKT_TYPE) !=
	    PKT_TYPE_RX_EVENT || rxd->seq ||
	    (rxd->eid != W103D_MCU_EVENT_RX_ADDBA &&
	     rxd->eid != W103D_MCU_EVENT_RX_DELBA))
		return false;

	if (rxd->eid == W103D_MCU_EVENT_RX_ADDBA) {
		struct mt7663s_w103d_rx_addba_event *event;
		u16 ba_param, ssn, win_size;

		if (skb->len < sizeof(*rxd) + sizeof(*event))
			goto malformed;
		event = (void *)(rxd + 1);
		wcid_idx = event->wcid;
		ba_param = le16_to_cpu(event->ba_param);
		tid = FIELD_GET(IEEE80211_ADDBA_PARAM_TID_MASK, ba_param);
		win_size = FIELD_GET(IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK,
				     ba_param);
		ssn = IEEE80211_SEQ_TO_SN(le16_to_cpu(event->start_seq));
		win_size = clamp_t(u16, win_size, 1, 64);

		mutex_lock(&mdev->mutex);
		wcid = rcu_dereference_protected(mdev->wcid[wcid_idx],
						 lockdep_is_held(&mdev->mutex));
		if (!wcid || !wcid->sta)
			ret = -ENOENT;
		else
			ret = mt76_rx_aggr_start(mdev, wcid, tid, ssn, win_size);
		mutex_unlock(&mdev->mutex);

		dev_info_ratelimited(mdev->dev,
			"W103D: firmware RX BA start wcid=%u tid=%u ssn=%u win=%u result=%d\n",
			wcid_idx, tid, ssn, win_size, ret);
	} else {
		struct mt7663s_w103d_rx_delba_event *event;

		if (skb->len < sizeof(*rxd) + sizeof(*event))
			goto malformed;
		event = (void *)(rxd + 1);
		wcid_idx = event->wcid;
		tid = event->tid;
		if (tid >= IEEE80211_NUM_TIDS)
			goto malformed;

		mutex_lock(&mdev->mutex);
		wcid = rcu_dereference_protected(mdev->wcid[wcid_idx],
						 lockdep_is_held(&mdev->mutex));
		if (wcid && wcid->sta)
			mt76_rx_aggr_stop(mdev, wcid, tid);
		else
			ret = -ENOENT;
		mutex_unlock(&mdev->mutex);

		dev_info_ratelimited(mdev->dev,
			"W103D: firmware RX BA stop wcid=%u tid=%u result=%d\n",
			wcid_idx, tid, ret);
	}

	dev_kfree_skb(skb);
	return true;

malformed:
	dev_warn_ratelimited(mdev->dev,
			     "W103D: malformed firmware RX BA event eid=%u len=%u\n",
			     rxd->eid, skb->len);
	dev_kfree_skb(skb);
	return true;
}

void mt7663s_w103d_queue_rx_skb(struct mt76_dev *mdev, enum mt76_rxq_id qid,
				 struct sk_buff *skb, u32 *info)
{
	if (mt7663s_w103d_active() &&
	    mt7663s_w103d_handle_rx_ba_event(mdev, skb))
		return;

	mt7615_queue_rx_skb(mdev, qid, skb, info);
}
