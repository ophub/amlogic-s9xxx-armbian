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
#include <linux/mt7663-combo.h>

#include <linux/mmc/host.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>

#include "../sdio.h"
#include "mt7615.h"
#include "mac.h"
#include "mcu.h"
#include "mt7663s_w103d.h"

struct mt7663s_dev {
	struct mt7615_dev dev;
	struct mt7663_combo *combo;
};

static struct mt7663_combo *mt7663s_combo(struct mt7615_dev *dev)
{
	return container_of(dev, struct mt7663s_dev, dev)->combo;
}

static const struct sdio_device_id mt7663s_table[] = {
	{ SDIO_DEVICE(SDIO_VENDOR_ID_MEDIATEK, 0x7603) },
	{ }	/* Terminating entry */
};

static void mt7663s_txrx_worker(struct mt76_worker *w)
{
	struct mt76_sdio *sdio = container_of(w, struct mt76_sdio,
					      txrx_worker);
	struct mt76_dev *mdev = container_of(sdio, struct mt76_dev, sdio);
	struct mt7615_dev *dev = container_of(mdev, struct mt7615_dev, mt76);

	if (!mt76_connac_pm_ref(&dev->mphy, &dev->pm)) {
		if (mt7663s_w103d_active())
			mt7663s_w103d_pm_ref_failed(mdev);
		queue_work(mdev->wq, &dev->pm.wake_work);
		return;
	}
	if (mt7663s_w103d_active()) {
		mt7663s_w103d_txrx_worker_enter(mdev);
		mt7663s_w103d_txrx_worker(mdev);
		if (mt7663s_w103d_txrx_worker_next(mdev) &&
		    !test_bit(MT76_MCU_RESET, &mdev->phy.state) &&
		    !test_bit(MT76_STATE_SUSPEND, &mdev->phy.state))
			mt76_worker_schedule(&sdio->txrx_worker);
	} else {
		mt76s_txrx_worker(sdio);
	}
	mt76_connac_pm_unref(&dev->mphy, &dev->pm);
}

static void mt7663s_bss_info_changed(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif,
				     struct ieee80211_bss_conf *info,
				     u64 changed)
{
	mt7615_ops.bss_info_changed(hw, vif, info, changed);
	mt7663s_w103d_note_assoc(hw, vif, changed);
}

static int
mt7663s_w103d_ampdu_action(struct ieee80211_hw *hw,
			   struct ieee80211_vif *vif,
			   struct ieee80211_ampdu_params *params)
{
	struct mt7615_dev *dev = mt7615_hw_dev(hw);
	struct ieee80211_sta *sta = params->sta;
	struct ieee80211_txq *txq = sta->txq[params->tid];
	struct mt7615_sta *msta = (struct mt7615_sta *)sta->drv_priv;
	struct mt76_txq *mtxq;
	u16 tid = params->tid;
	int ret = 0;

	if (!txq)
		return -EINVAL;

	mtxq = (struct mt76_txq *)txq->drv_priv;
	mt7615_mutex_acquire(dev);

	switch (params->action) {
	case IEEE80211_AMPDU_RX_START:
		mt76_rx_aggr_start(&dev->mt76, &msta->wcid, tid, params->ssn,
				   params->buf_size);
		ret = mt7615_mcu_add_rx_ba(dev, params, true);
		break;
	case IEEE80211_AMPDU_RX_STOP:
		mt76_rx_aggr_stop(&dev->mt76, &msta->wcid, tid);
		ret = mt7615_mcu_add_rx_ba(dev, params, false);
		break;
	case IEEE80211_AMPDU_TX_OPERATIONAL:
		mtxq->aggr = true;
		mtxq->send_bar = false;
		ret = mt7615_mcu_add_tx_ba(dev, params, true);
		ieee80211_send_bar(vif, sta->addr, tid, mtxq->agg_ssn);
		break;
	case IEEE80211_AMPDU_TX_STOP_FLUSH:
	case IEEE80211_AMPDU_TX_STOP_FLUSH_CONT:
		mtxq->aggr = false;
		ret = mt7615_mcu_add_tx_ba(dev, params, false);
		break;
	case IEEE80211_AMPDU_TX_START:
		/* mt7615's stock callback reads the starting sequence number from
		 * WTBL word 11.  W103D cannot access that 0x82xxxx register window;
		 * use mac80211's authoritative software SSN, as other mt76 drivers
		 * do, instead of negotiating ADDBA with a mailbox-error value.
		 */
		mtxq->agg_ssn = IEEE80211_SN_TO_SEQ(params->ssn);
		ret = IEEE80211_AMPDU_TX_START_IMMEDIATE;
		break;
	case IEEE80211_AMPDU_TX_STOP_CONT:
		mtxq->aggr = false;
		ret = mt7615_mcu_add_tx_ba(dev, params, false);
		ieee80211_stop_tx_ba_cb_irqsafe(vif, sta->addr, tid);
		break;
	}

	dev_info_ratelimited(dev->mt76.dev,
		"W103D: AMPDU action=%u tid=%u ssn=%u result=%d\n",
		params->action, tid, params->ssn, ret);
	mt7615_mutex_release(dev);

	return ret;
}

static void mt7663s_init_work(struct work_struct *work)
{
	struct mt7615_dev *dev;
	unsigned int filter_flags = 0;
	int ret;

	dev = container_of(work, struct mt7615_dev, mcu_work);
	ret = mt7663_combo_begin(mt7663s_combo(dev));
	if (ret) {
		dev_err(dev->mt76.dev, "W103D: combo setup unavailable: %d\n", ret);
		return;
	}
	ret = mt7663s_mcu_init(dev);
	if (ret) {
		dev_err(dev->mt76.dev, "MCU initialization failed: %d\n", ret);
		goto out;
	}

	mt7615_init_work(dev);
	mt7663s_w103d_configure_filter(dev->mt76.hw, 0, &filter_flags, 0);
	mt7663s_w103d_limit_vht80(dev);
	mt7663s_w103d_init_mac_work(dev);
out:
	mt7663_combo_end(mt7663s_combo(dev));
}

static int mt7663s_parse_intr(struct mt76_dev *dev, struct mt76s_intr *intr)
{
	struct mt76_sdio *sdio = &dev->sdio;
	struct mt7663s_intr *irq_data = sdio->intr_data;
	int i, err;

	sdio_claim_host(sdio->func);
	err = sdio_readsb(sdio->func, irq_data, MCR_WHISR, sizeof(*irq_data));
	sdio_release_host(sdio->func);

	if (err)
		return err;

	intr->isr = irq_data->isr;
	intr->rec_mb = irq_data->rec_mb;
	intr->tx.wtqcr = irq_data->tx.wtqcr;
	intr->rx.num = irq_data->rx.num;
	for (i = 0; i < 2 ; i++)
		intr->rx.len[i] = irq_data->rx.len[i];

	return 0;
}

static int mt7663s_w103d_configure_rx_enhance(struct sdio_func *func)
{
	u32 ctrl;
	int ret;

	sdio_claim_host(func);
	ctrl = sdio_readl(func, MCR_WHCR, &ret);
	if (!ret) {
		/* MT7663 encodes the full 16-entry enhanced RX length table as
		 * zero in MAX_HIF_RX_LEN_NUM, matching the vendor gen4m path.
		 */
		ctrl &= ~(MAX_HIF_RX_LEN_NUM | W_INT_CLR_CTRL);
		ctrl |= RX_ENHANCE_MODE;
		sdio_writel(func, ctrl, MCR_WHCR, &ret);
	}
	sdio_release_host(func);

	return ret;
}

static int mt7663s_probe(struct sdio_func *func,
			 const struct sdio_device_id *id)
{
	static const struct mt76_driver_ops drv_ops = {
		.txwi_size = MT_USB_TXD_SIZE,
		.drv_flags = MT_DRV_RX_DMA_HDR,
		.tx_prepare_skb = mt7663s_w103d_tx_prepare_skb,
		.tx_complete_skb = mt7663_usb_sdio_tx_complete_skb,
		/* MT7663 SDIO must not issue per-batch WTBL airtime reads. */
		.tx_status_data = NULL,
		.rx_skb = mt7663s_w103d_queue_rx_skb,
		.rx_check = mt7663s_w103d_rx_check,
		.sta_add = mt7615_mac_sta_add,
		.sta_remove = mt7615_mac_sta_remove,
		.update_survey = mt7615_update_channel,
		.set_channel = mt7663s_w103d_set_channel,
	};
	static const struct mt76_bus_ops mt7663s_ops = {
		.rr = mt76s_rr,
		.rmw = mt76s_rmw,
		.wr = mt76s_wr,
		.write_copy = mt76s_write_copy,
		.read_copy = mt76s_read_copy,
		.wr_rp = mt76s_wr_rp,
		.rd_rp = mt76s_rd_rp,
		.type = MT76_BUS_SDIO,
	};
	struct ieee80211_ops *ops;
	struct mt7615_dev *dev;
	struct mt76_dev *mdev;
	struct mt7663_combo *combo;
	int ret;

	combo = mt7663_combo_get(func);
	if (IS_ERR(combo))
		return dev_err_probe(&func->dev, PTR_ERR(combo),
				     "waiting for Bluetooth supplier\n");
	ret = mt7663_combo_begin(combo);
	if (ret) {
		mt7663_combo_put(combo);
		return dev_err_probe(&func->dev, ret,
				     "Bluetooth setup is not ready\n");
	}

	ops = devm_kmemdup(&func->dev, &mt7615_ops, sizeof(mt7615_ops),
			   GFP_KERNEL);
	if (!ops) {
		ret = -ENOMEM;
		goto combo_error;
	}

	mdev = mt76_alloc_device(&func->dev, sizeof(struct mt7663s_dev),
				ops, &drv_ops);
	if (!mdev) {
		ret = -ENOMEM;
		goto combo_error;
	}

	dev = container_of(mdev, struct mt7615_dev, mt76);
	container_of(dev, struct mt7663s_dev, dev)->combo = combo;
	if (mt7663s_w103d_active()) {
		/* The MT7663 V3 SDIO firmware owns Block Ack negotiation and
		 * consumes BA action frames before they reach the host.  Mark TX BA
		 * setup as firmware-owned so mac80211 does not start a second
		 * negotiation and wait forever for the consumed ADDBA response.
		 */
		ieee80211_hw_set(mdev->hw, TX_AMPDU_SETUP_IN_HW);
		mdev->hw->max_tx_fragments = 1;
		/* SD_EMMC_B uses DRAM descriptor DMA and accepts large requests. */
		mdev->hw->max_mtu = W103D_SAFE_MTU;
		ops->configure_filter = mt7663s_w103d_configure_filter;
		ops->bss_info_changed = mt7663s_bss_info_changed;
		ops->hw_scan = mt7663s_w103d_hw_scan;
		ops->cancel_hw_scan = mt7663s_w103d_cancel_hw_scan;
		ops->set_key = mt7663s_w103d_set_key;
		ops->ampdu_action = mt7663s_w103d_ampdu_action;
	}

	INIT_WORK(&dev->mcu_work, mt7663s_init_work);
	dev->reg_map = mt7663_usb_sdio_reg_map;
	dev->ops = ops;
	sdio_set_drvdata(func, dev);

	ret = mt76s_init(mdev, func, &mt7663s_ops);
	if (ret < 0)
		goto error;

	ret = mt76s_hw_init(mdev, func, MT76_CONNAC_SDIO);
	if (ret)
		goto error;

	if (mt7663s_w103d_active()) {
		ret = mt7663s_w103d_configure_rx_enhance(func);
		if (ret)
			goto error;

		dev_info(mdev->dev,
			 "W103D: enabled vendor 16-entry RX enhance mode\n");
	}

	mdev->rev = (mt76_rr(dev, MT_HW_CHIPID) << 16) |
		    (mt76_rr(dev, MT_HW_REV) & 0xff);
	dev_dbg(mdev->dev, "ASIC revision: %04x\n", mdev->rev);

	if (mt7663s_w103d_active()) {
		mdev->sdio.parse_irq = mt7663s_w103d_parse_intr;
		ret = mt7663s_w103d_init_intr(mdev);
		if (ret)
			goto error;

		sdio_claim_host(func);
		sdio_release_irq(func);
		ret = sdio_claim_irq(func, mt7663s_w103d_sdio_irq);
		sdio_release_host(func);
		if (ret)
			goto error;
	} else {
		mdev->sdio.parse_irq = mt7663s_parse_intr;
		mdev->sdio.intr_data = devm_kmalloc(mdev->dev,
						    sizeof(struct mt7663s_intr),
						    GFP_KERNEL);
		if (!mdev->sdio.intr_data) {
			ret = -ENOMEM;
			goto error;
		}
	}

	ret = mt76s_alloc_rx_queue(mdev, MT_RXQ_MAIN);
	if (ret)
		goto error;

	if (mt7663s_w103d_active()) {
		ret = mt76s_alloc_rx_queue(mdev, MT_RXQ_MCU);
		if (ret)
			goto error;
	}

	ret = mt76s_alloc_tx(mdev);
	if (ret)
		goto error;

	ret = mt76_worker_setup(mt76_hw(dev), &mdev->sdio.txrx_worker,
				mt7663s_txrx_worker, "sdio-txrx");
	if (ret)
		goto error;

	if (!mt7663s_w103d_active())
		sched_set_fifo_low(mdev->sdio.txrx_worker.task);

	ret = mt7663_usb_sdio_register_device(dev);
	if (ret)
		goto error;

	mt7663_combo_end(combo);
	return 0;

error:
	if (mt7663s_w103d_active())
		mt7663s_w103d_deinit_intr(&dev->mt76);
	mt76s_deinit(&dev->mt76);
	sdio_claim_host(func);
	sdio_disable_func(func);
	sdio_release_host(func);
	sdio_set_drvdata(func, NULL);
	mt76_free_device(&dev->mt76);
combo_error:
	mt7663_combo_end(combo);
	mt7663_combo_put(combo);
	return ret;
}

static void mt7663s_remove(struct sdio_func *func)
{
	struct mt7615_dev *dev = sdio_get_drvdata(func);
	struct mt7663_combo *combo = mt7663s_combo(dev);
	bool running;
	int ret, combo_ret;

	/* MCU setup is asynchronous.  Keep its transport and IRQ state alive
	 * until setup, mac80211 teardown and the firmware restart have finished.
	 */
	running = mt7615_wait_for_mcu_init(dev);
	combo_ret = mt7663_combo_begin(combo);
	ieee80211_unregister_hw(dev->mt76.hw);
	if (mt7663s_w103d_active() && running && !combo_ret) {
		ret = mt7663s_w103d_mcu_restart(&dev->mt76);
		dev_info(&func->dev, "W103D: firmware exit result=%d\n", ret);
		clear_bit(MT76_STATE_MCU_RUNNING, &dev->mphy.state);
		skb_queue_purge(&dev->mt76.mcu.res_q);
	}

	clear_bit(MT76_STATE_INITIALIZED, &dev->mphy.state);
	if (mt7663s_w103d_active())
		mt7663s_w103d_deinit_intr(&dev->mt76);

	mt76s_deinit(&dev->mt76);
	sdio_claim_host(func);
	sdio_disable_func(func);
	sdio_release_host(func);
	sdio_set_drvdata(func, NULL);
	mt76_free_device(&dev->mt76);
	if (!combo_ret)
		mt7663_combo_end(combo);
	mt7663_combo_put(combo);
}

static int mt7663s_suspend(struct device *dev)
{
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct mt7615_dev *mdev = sdio_get_drvdata(func);
	int err;

	if (!test_bit(MT76_STATE_SUSPEND, &mdev->mphy.state) &&
	    mt7615_firmware_offload(mdev)) {
		int err;

		err = mt76_connac_mcu_set_hif_suspend(&mdev->mt76, true, true);
		if (err < 0)
			return err;
	}

	sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);

	err = mt7615_mcu_set_fw_ctrl(mdev);
	if (err)
		return err;

	mt76_worker_disable(&mdev->mt76.sdio.txrx_worker);
	/* Stop the producer before draining its delayed watchdog. */
	if (mt7663s_w103d_active())
		mt7663s_w103d_cancel_quota_watchdog_sync(&mdev->mt76);

	mt76_worker_disable(&mdev->mt76.sdio.status_worker);
	mt76_worker_disable(&mdev->mt76.sdio.net_worker);
	mt76_worker_disable(&mdev->mt76.sdio.stat_worker);

	clear_bit(MT76_READING_STATS, &mdev->mphy.state);

	mt76_tx_status_check(&mdev->mt76, true);

	return 0;
}

static int mt7663s_resume(struct device *dev)
{
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct mt7615_dev *mdev = sdio_get_drvdata(func);
	int err;

	mt76_worker_enable(&mdev->mt76.sdio.txrx_worker);
	mt76_worker_enable(&mdev->mt76.sdio.status_worker);
	mt76_worker_enable(&mdev->mt76.sdio.net_worker);

	err = mt7615_mcu_set_drv_ctrl(mdev);
	if (err)
		return err;

	if (!test_bit(MT76_STATE_SUSPEND, &mdev->mphy.state) &&
	    mt7615_firmware_offload(mdev))
		err = mt76_connac_mcu_set_hif_suspend(&mdev->mt76, false, true);

	return err;
}

MODULE_DEVICE_TABLE(sdio, mt7663s_table);
MODULE_FIRMWARE(MT7663_OFFLOAD_FIRMWARE_N9);
MODULE_FIRMWARE(MT7663_OFFLOAD_ROM_PATCH);
MODULE_FIRMWARE(MT7663_FIRMWARE_N9);
MODULE_FIRMWARE(MT7663_ROM_PATCH);

static DEFINE_SIMPLE_DEV_PM_OPS(mt7663s_pm_ops, mt7663s_suspend, mt7663s_resume);

static struct sdio_driver mt7663s_driver = {
	.name		= KBUILD_MODNAME,
	.probe		= mt7663s_probe,
	.remove		= mt7663s_remove,
	.id_table	= mt7663s_table,
	.drv.pm		= pm_sleep_ptr(&mt7663s_pm_ops),
};
module_sdio_driver(mt7663s_driver);

MODULE_AUTHOR("Sean Wang <sean.wang@mediatek.com>");
MODULE_AUTHOR("Lorenzo Bianconi <lorenzo@kernel.org>");
MODULE_DESCRIPTION("MediaTek MT7663S (SDIO) wireless driver");
MODULE_LICENSE("Dual BSD/GPL");
