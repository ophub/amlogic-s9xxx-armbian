// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 MediaTek Inc.
 *
 * Author: Felix Fietkau <nbd@nbd.name>
 *	   Lorenzo Bianconi <lorenzo@kernel.org>
 *	   Sean Wang <sean.wang@mediatek.com>
 */

#include <linux/kernel.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/unaligned.h>

#include <linux/mmc/host.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>

#include "trace.h"
#include "sdio.h"
#include "mt76.h"
#include "mt7663s_w103d.h"

#define W103D_TX_FRAMES_PER_QUEUE 32
#define W103D_TX_WORKER_ROUNDS 8

static int mt76s_refill_sched_quota(struct mt76_dev *dev, u32 *data)
{
	u32 ple_ac_data_quota[] = {
		FIELD_GET(TXQ_CNT_L, data[4]), /* VO */
		FIELD_GET(TXQ_CNT_H, data[3]), /* VI */
		FIELD_GET(TXQ_CNT_L, data[3]), /* BE */
		FIELD_GET(TXQ_CNT_H, data[2]), /* BK */
	};
	u32 pse_ac_data_quota[] = {
		FIELD_GET(TXQ_CNT_H, data[1]), /* VO */
		FIELD_GET(TXQ_CNT_L, data[1]), /* VI */
		FIELD_GET(TXQ_CNT_H, data[0]), /* BE */
		FIELD_GET(TXQ_CNT_L, data[0]), /* BK */
	};
	u32 pse_mcu_quota = FIELD_GET(TXQ_CNT_L, data[2]);
	u32 pse_data_quota = 0, ple_data_quota = 0;
	struct mt76_sdio *sdio = &dev->sdio;
	int i;

	for (i = 0; i < ARRAY_SIZE(pse_ac_data_quota); i++) {
		pse_data_quota += pse_ac_data_quota[i];
		ple_data_quota += ple_ac_data_quota[i];
	}
	/* Every read-clear completion snapshot is consumed exactly once. */
	memset(data, 0, 8 * sizeof(*data));

	if (!pse_data_quota && !ple_data_quota && !pse_mcu_quota)
		return 0;

	sdio->sched.pse_mcu_quota += pse_mcu_quota;
	sdio->sched.pse_data_quota += pse_data_quota;
	sdio->sched.ple_data_quota += ple_data_quota;

	return pse_data_quota + ple_data_quota + pse_mcu_quota;
}

static struct sk_buff *
mt76s_build_rx_skb(void *data, int data_len, int buf_len)
{
	int len = min_t(int, data_len, MT_SKB_HEAD_LEN);
	struct sk_buff *skb;

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return NULL;

	skb_put_data(skb, data, len);
	if (data_len > len) {
		struct page *page;

		data += len;
		page = virt_to_head_page(data);
		skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags,
				page, data - page_address(page),
				data_len - len, buf_len);
		get_page(page);
	}

	return skb;
}

static int
mt76s_rx_run_queue(struct mt76_dev *dev, enum mt76_rxq_id qid,
		   struct mt76s_intr *intr)
{
	struct mt76_queue *q = &dev->q_rx[qid];
	struct mt76_sdio *sdio = &dev->sdio;
	u16 lengths[16];
	int num = intr->rx.num[qid];
	int data_len = 0, len, err, i, queued = 0;
	struct page *page;
	u8 *buf, *end;

	if (!num)
		return 0;
	if (num > ARRAY_SIZE(lengths))
		return -EPROTO;
	memcpy(lengths, intr->rx.len[qid], num * sizeof(*lengths));
	for (i = 0; i < num; i++) {
		if (lengths[i] < sizeof(__le32))
			return -EPROTO;
		data_len += round_up(lengths[i] + 4, 4);
	}

	/* The RX-enhance trailer is before bus padding, not at its end. */
	len = data_len + W103D_RX_ENHANCE_PAD + W103D_RX_ENHANCE_SIZE;

	if (len > sdio->func->cur_blksize)
		len = roundup(len, sdio->func->cur_blksize);

	page = __dev_alloc_pages(GFP_KERNEL, get_order(len));
	if (!page)
		return -ENOMEM;

	buf = page_address(page);

	sdio_claim_host(sdio->func);
	err = sdio_readsb(sdio->func, buf, MCR_WRDR(qid), len);
	sdio_release_host(sdio->func);

	if (err < 0) {
		dev_err(dev->dev, "sdio read data failed:%d\n", err);
		put_page(page);
		return err;
	}

	end = buf + data_len;
	if (get_unaligned_le32(end)) {
		err = -EPROTO;
		goto invalid_tail;
	}
	err = mt7663s_w103d_rx_enhance(dev, intr,
				     end + W103D_RX_ENHANCE_PAD);
	if (err)
		goto invalid_tail;

	/* Hardware packets and enqueued skbs are different counts: rx_check
	 * consumes TXS packets without adding an skb to the receive ring.
	 */
	for (i = 0; i < num; i++) {
		int span = round_up(lengths[i] + 4, 4);
		int index = (q->head + queued) % q->ndesc;
		struct mt76_queue_entry *e = &q->entry[index];
		__le32 *rxd = (__le32 *)buf;

		/* parse rxd to get the actual packet length */
		len = le32_get_bits(rxd[0], GENMASK(15, 0));
		if (unlikely(len < sizeof(*rxd) || len > lengths[i] ||
			     span > end - buf)) {
			dev_err_ratelimited(dev->dev,
				"W103D: invalid RX packet length %d/%u\n",
				len, lengths[i]);
			goto next;
		}

		/* Optimized path for TXS */
		if (!dev->drv->rx_check || dev->drv->rx_check(dev, buf, len)) {
			if (q->queued + queued >= q->ndesc)
				goto next;
			e->skb = mt76s_build_rx_skb(buf, len,
						    span);
			if (!e->skb)
				goto next;

			queued++;
		}
next:
		buf += span;
	}
	put_page(page);

	spin_lock_bh(&q->lock);
	q->head = (q->head + queued) % q->ndesc;
	q->queued += queued;
	spin_unlock_bh(&q->lock);

	if (queued)
		mt76_worker_schedule(&sdio->net_worker);
	return num;

invalid_tail:
	dev_err_ratelimited(dev->dev, "W103D: invalid RX enhance trailer\n");
	put_page(page);
	return err;
}

static int mt76s_consume_intr(struct mt76_dev *dev, struct mt76s_intr *intr)
{
	u32 sw = intr->isr & WHIER_D2H_SW_INT &
		 ~(H2D_SW_INT_READ | H2D_SW_INT_WRITE);
	int credits = mt76s_refill_sched_quota(dev, intr->tx.wtqcr);

	/* Preserve visibility of firmware software events; they must not be
	 * mistaken for RX packets or replayed on every pending iteration.
	 */
	if (sw)
		dev_warn_ratelimited(dev->dev,
			"W103D: firmware interrupt %#x mailbox %#x/%#x\n",
			sw, intr->rec_mb[0], intr->rec_mb[1]);
	intr->isr &= WHIER_RX0_DONE_INT_EN | WHIER_RX1_DONE_INT_EN;
	return !!credits;
}

static int mt76s_rx_handler(struct mt76_dev *dev)
{
	struct mt76_sdio *sdio = &dev->sdio;
	struct mt76s_intr intr;
	int nframes = 0, ret, qid;

	/* Serialize status, both RX ports and publication of the final tail
	 * against register-mailbox WHISR reads. Nested SDIO claims are allowed.
	 */
	sdio_claim_host(sdio->func);
	ret = sdio->parse_irq(dev, &intr);
	if (ret)
		goto out;

	trace_dev_irq(dev, intr.isr, 0);

	nframes += mt76s_consume_intr(dev, &intr);
	for (qid = 0; qid < 2; qid++) {
		if (!intr.rx.num[qid]) {
			intr.isr &= ~(qid ? WHIER_RX1_DONE_INT_EN :
					   WHIER_RX0_DONE_INT_EN);
			continue;
		}
		ret = mt76s_rx_run_queue(dev, qid, &intr);
		if (ret < 0) {
			/* Allocation failure did not touch the FIFO: preserve its view.
			 * Protocol/transfer errors cannot safely replay the old read.
			 */
			if (ret != -ENOMEM) {
				dev_err_ratelimited(dev->dev,
					"W103D: RX%d transaction failed: %d\n", qid, ret);
				goto out;
			}
			break;
		}
		nframes += ret;
		/* RX0's new lengths now describe RX1, as in the vendor driver. */
		nframes += mt76s_consume_intr(dev, &intr);
	}
	mt7663s_w103d_save_intr(dev, &intr);
out:
	sdio_release_host(sdio->func);
	return ret < 0 ? ret : nframes;
}

static int
mt76s_tx_pick_quota(struct mt76_sdio *sdio, bool mcu, int buf_sz,
		    int *pse_size, int *ple_size)
{
	int pse_sz;

	pse_sz = DIV_ROUND_UP(buf_sz + sdio->sched.deficit,
			      sdio->sched.pse_page_size);

	if (mcu && sdio->hw_ver == MT76_CONNAC2_SDIO)
		pse_sz = 1;

	if (mcu) {
		if (sdio->sched.pse_mcu_quota < *pse_size + pse_sz)
			return -EBUSY;
	} else {
		if (sdio->sched.pse_data_quota < *pse_size + pse_sz ||
		    sdio->sched.ple_data_quota < *ple_size + 1)
			return -EBUSY;

		*ple_size = *ple_size + 1;
	}
	*pse_size = *pse_size + pse_sz;

	return 0;
}

static void
mt76s_tx_update_quota(struct mt76_sdio *sdio, bool mcu, int pse_size,
		      int ple_size)
{
	if (mcu) {
		sdio->sched.pse_mcu_quota -= pse_size;
	} else {
		sdio->sched.pse_data_quota -= pse_size;
		sdio->sched.ple_data_quota -= ple_size;
	}
}

static int __mt76s_xmit_queue(struct mt76_dev *dev, u8 *data, int len)
{
	struct mt76_sdio *sdio = &dev->sdio;
	int err;

	if (len > sdio->func->cur_blksize)
		len = roundup(len, sdio->func->cur_blksize);

	sdio_claim_host(sdio->func);
	err = sdio_writesb(sdio->func, MCR_WTDR1, data, len);
	sdio_release_host(sdio->func);

	if (err)
		dev_err(dev->dev, "sdio write failed: %d\n", err);

	return err;
}

static int mt76s_tx_run_queue(struct mt76_dev *dev, struct mt76_queue *q)
{
	int err, nframes = 0, len = 0, pse_sz = 0, ple_sz = 0;
	bool mcu;
	struct mt76_sdio *sdio;
	bool polled = false;
	u8 pad;

	if (!q)
		return 0;

	mcu = q == dev->q_mcu[MT_MCUQ_WM];
	sdio = &dev->sdio;

	while (q->first != q->head && nframes < W103D_TX_FRAMES_PER_QUEUE) {
		struct mt76_queue_entry *e = &q->entry[q->first];
		struct sk_buff *iter;

		smp_rmb();

		if (test_bit(MT76_MCU_RESET, &dev->phy.state))
			goto next;

		if (!test_bit(MT76_STATE_MCU_RUNNING, &dev->phy.state)) {
			__skb_put_zero(e->skb, 4);
			err = __skb_grow(e->skb, roundup(e->skb->len,
							 sdio->func->cur_blksize));
			if (err)
				return err;
			err = __mt76s_xmit_queue(dev, e->skb->data,
						 e->skb->len);
			if (err)
				return err;

			goto next;
		}

		pad = roundup(e->skb->len, 4) - e->skb->len;
		if (len + e->skb->len + pad + 4 > dev->sdio.xmit_buf_sz)
			break;

		if (mt76s_tx_pick_quota(sdio, mcu, e->buf_sz, &pse_sz,
					&ple_sz)) {
			if (!polled) {
				polled = true;
				if (mt7663s_w103d_poll_tx_quota(dev) > 0 &&
				    !mt76s_tx_pick_quota(sdio, mcu, e->buf_sz,
							 &pse_sz, &ple_sz))
					goto quota_ok;
			}
			break;
		}
quota_ok:
		memcpy(sdio->xmit_buf + len, e->skb->data, skb_headlen(e->skb));
		len += skb_headlen(e->skb);
		nframes++;

		skb_walk_frags(e->skb, iter) {
			memcpy(sdio->xmit_buf + len, iter->data, iter->len);
			len += iter->len;
			nframes++;
		}

		if (unlikely(pad)) {
			memset(sdio->xmit_buf + len, 0, pad);
			len += pad;
		}
next:
		q->first = (q->first + 1) % q->ndesc;
		e->done = true;
	}

	if (nframes) {
		memset(sdio->xmit_buf + len, 0, 4);
		err = __mt76s_xmit_queue(dev, sdio->xmit_buf, len + 4);
		if (err) {
			mt76_worker_schedule(&sdio->status_worker);
			return err;
		}
	}
	mt76s_tx_update_quota(sdio, mcu, pse_sz, ple_sz);

	mt76_worker_schedule(&sdio->status_worker);

	return nframes;
}

void mt7663s_w103d_txrx_worker_core(struct mt76_dev *dev)
{
	struct mt76_sdio *sdio = &dev->sdio;
	int i, nframes, ret, rounds = 0;
	int tx_retries = 0;

	/* disable interrupt */
	sdio_claim_host(sdio->func);
	sdio_writeb(sdio->func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, NULL);
	sdio_release_host(sdio->func);

	do {
		nframes = 0;

		/* tx */
		for (i = 0; i <= MT_TXQ_PSD; i++) {
			ret = mt76s_tx_run_queue(dev, dev->phy.q_tx[i]);
			if (ret > 0)
				nframes += ret;
		}
		ret = mt76s_tx_run_queue(dev, dev->q_mcu[MT_MCUQ_WM]);
		if (ret > 0)
			nframes += ret;

		/* rx */
		ret = mt76s_rx_handler(dev);
		if (ret > 0)
			nframes += ret;

		if (test_bit(MT76_MCU_RESET, &dev->phy.state) ||
		    test_bit(MT76_STATE_SUSPEND, &dev->phy.state)) {
			wake_up(&sdio->wait);
			break;
		}

		/* If no frames were transferred this round but TX queues still
		 * hold unsent packets waiting for in-flight air transmission
		 * to return credits, perform bounded microsecond retry while
		 * keeping RX drained.
		 */
		if (!nframes && !mt7663s_w103d_txqs_empty(dev) &&
		    tx_retries < W103D_TX_QUOTA_RETRY_MAX) {
			tx_retries++;
			usleep_range(100, 200);
			ret = mt76s_rx_handler(dev);
			if (ret > 0)
				nframes += ret;
			ret = mt7663s_w103d_poll_tx_quota(dev);
			if (ret > 0)
				nframes += ret;
			if (nframes > 0)
				continue;
		}

		cond_resched();
		rounds++;
	} while (nframes > 0 && rounds < W103D_TX_WORKER_ROUNDS);

	/* The outer worker checks pending state before enabling interrupts. */

	if (!test_bit(MT76_MCU_RESET, &dev->phy.state) &&
	    !test_bit(MT76_STATE_SUSPEND, &dev->phy.state) &&
	    !mt7663s_w103d_txqs_empty(dev)) {
		if (nframes > 0 && rounds >= W103D_TX_WORKER_ROUNDS)
			mt76_worker_schedule(&sdio->txrx_worker);
		else
			mt7663s_w103d_arm_quota_watchdog(dev);
	} else {
		mt7663s_w103d_cancel_quota_watchdog(dev);
	}
}
