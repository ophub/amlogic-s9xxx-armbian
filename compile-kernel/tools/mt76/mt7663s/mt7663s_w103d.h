/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MT7663S_W103D_H
#define __MT7663S_W103D_H

struct mt76_dev;
struct mt76_sdio;
struct mt76s_intr;
struct mt7615_dev;
struct mt7615_mcu_ops;
struct sk_buff;
struct sdio_func;
struct mt76_tx_info;
struct mt76_wcid;
struct ieee80211_sta;
struct ieee80211_hw;
struct ieee80211_vif;
struct ieee80211_bss_conf;
struct ieee80211_scan_request;
enum mt76_txq_id;

bool mt7663s_w103d_active(void);
int mt7663s_w103d_mcu_restart(struct mt76_dev *mdev);
int mt7663s_w103d_init_intr(struct mt76_dev *mdev);
int mt7663s_w103d_parse_intr(struct mt76_dev *mdev,
			     struct mt76s_intr *intr);
int mt7663s_w103d_rx_enhance(struct mt76_dev *mdev,
			   struct mt76s_intr *intr, const void *data);
void mt7663s_w103d_save_intr(struct mt76_dev *mdev,
			   struct mt76s_intr *intr);
#define W103D_RX_ENHANCE_SIZE 112
#define W103D_RX_ENHANCE_PAD 4
void mt7663s_w103d_txrx_worker_enter(struct mt76_dev *mdev);
bool mt7663s_w103d_txrx_worker_next(struct mt76_dev *mdev);
void mt7663s_w103d_pm_ref_failed(struct mt76_dev *mdev);
void mt7663s_w103d_txrx_worker(struct mt76_dev *mdev);
void mt7663s_w103d_txrx_worker_core(struct mt76_dev *mdev);
void mt7663s_w103d_init_mac_work(struct mt7615_dev *dev);
void mt7663s_w103d_limit_vht80(struct mt7615_dev *dev);
unsigned int mt7663s_w103d_fw_headroom(struct mt76_dev *mdev);
u32 mt7663s_w103d_reg_rr(struct mt76_dev *mdev, u32 offset);
void mt7663s_w103d_reg_wr(struct mt76_dev *mdev, u32 offset, u32 value);
int mt7663s_w103d_mcu_send_msg(struct mt76_dev *mdev, int cmd,
			      const void *data, int len, bool wait_resp);
int mt7663s_w103d_mcu_send_message(struct mt76_dev *mdev,
					struct sk_buff *skb, int cmd, int *seq);
int mt7663s_w103d_set_channel(struct mt76_phy *mphy);
void mt7663s_w103d_configure_filter(struct ieee80211_hw *hw,
				    unsigned int changed_flags,
				    unsigned int *total_flags,
				    u64 multicast);
void mt7663s_w103d_note_assoc(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif, u64 changed);
int mt7663s_w103d_hw_scan(struct ieee80211_hw *hw,
			  struct ieee80211_vif *vif,
			  struct ieee80211_scan_request *req);
void mt7663s_w103d_cancel_hw_scan(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif);
int mt7663s_w103d_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			  struct ieee80211_vif *vif,
			  struct ieee80211_sta *sta,
			  struct ieee80211_key_conf *key);
void mt7663s_w103d_sdio_irq(struct sdio_func *func);
bool mt7663s_w103d_rx_check(struct mt76_dev *mdev, void *data, int len);
void mt7663s_w103d_queue_rx_skb(struct mt76_dev *mdev, enum mt76_rxq_id qid,
				 struct sk_buff *skb, u32 *info);
bool mt7663s_w103d_check_tx_size(struct mt76_dev *mdev, struct sk_buff *skb);
int mt7663s_w103d_tx_prepare_skb(struct mt76_dev *mdev, void *txwi_ptr,
					 enum mt76_txq_id qid, struct mt76_wcid *wcid,
					 struct ieee80211_sta *sta,
					 struct mt76_tx_info *tx_info);
void mt7663s_w103d_purge_stale_responses(struct mt76_dev *mdev);
bool mt7663s_w103d_should_wait_resp(int cmd);
void mt7663s_w103d_install_legacy_ops(struct mt7615_mcu_ops *ops);

#define W103D_TX_PAGE_PER_FRAME		16
#define W103D_QUOTA_WATCHDOG_MS		4
#define W103D_TX_QUOTA_RETRY_MAX	3
#define W103D_SAFE_MTU			1500

int mt7663s_w103d_poll_tx_quota(struct mt76_dev *mdev);
void mt7663s_w103d_arm_quota_watchdog(struct mt76_dev *mdev);
void mt7663s_w103d_cancel_quota_watchdog(struct mt76_dev *mdev);
void mt7663s_w103d_cancel_quota_watchdog_sync(struct mt76_dev *mdev);
void mt7663s_w103d_deinit_intr(struct mt76_dev *mdev);
bool mt7663s_w103d_txqs_empty(struct mt76_dev *mdev);

#endif
