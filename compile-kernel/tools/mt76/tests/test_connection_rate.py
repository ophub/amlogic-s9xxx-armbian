"""Exercise the real TXWI encoder for connection probes and ordinary VHT data."""
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
from test_rx_enhance import DRIVER, function


@unittest.skipUnless(os.name == 'posix', 'requires Linux GCC')
class ConnectionProbeRate(unittest.TestCase):
    def test_probe_descriptor_and_data_rate_adaptation(self):
        prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t __le32;
#define BIT(n) (1U << (n))
#define GENMASK(h,l) ((~0U >> (31-(h))) & (~0U << (l)))
#define FIELD_PREP(m,v) (((u32)(v) << __builtin_ctz(m)) & (m))
#define FIELD_GET(m,v) (((u32)(v) & (m)) >> __builtin_ctz(m))
#define cpu_to_le32(v) ((u32)(v))
#define clamp_t(t,v,l,h) ((t)(v) < (l) ? (l) : ((t)(v) > (h) ? (h) : (t)(v)))
#define rcu_read_lock() do {} while (0)
#define rcu_read_unlock() do {} while (0)
#define rcu_dereference(v) (v)
#define IEEE80211_TX_RC_MCS BIT(0)
#define IEEE80211_TX_RC_VHT_MCS BIT(1)
#define IEEE80211_TX_RC_40_MHZ_WIDTH BIT(2)
#define IEEE80211_TX_RC_80_MHZ_WIDTH BIT(3)
#define IEEE80211_TX_RC_GREEN_FIELD BIT(4)
#define IEEE80211_TX_RC_USE_SHORT_PREAMBLE BIT(5)
#define IEEE80211_TX_RC_SHORT_GI BIT(6)
#define IEEE80211_TX_CTL_USE_MINRATE BIT(0)
#define IEEE80211_TX_CTL_RATE_CTRL_PROBE BIT(1)
#define IEEE80211_TX_CTL_STBC BIT(2)
#define IEEE80211_TX_CTL_LDPC BIT(3)
/* MT7615/MT7663 descriptor wire definitions from mt7615/mac.h. */
#define MT_TXD2_FIX_RATE BIT(31)
#define MT_TXD2_BA_DISABLE BIT(29)
#define MT_TXD3_REM_TX_COUNT GENMASK(15,11)
#define MT_TXD6_SGI BIT(30)
#define MT_TXD6_LDPC BIT(29)
#define MT_TXD6_TX_RATE GENMASK(27,16)
#define MT_TXD6_FIXED_BW BIT(2)
#define MT_TXD6_BW GENMASK(1,0)
#define MT_TX_RATE_STBC BIT(11)
#define MT_TX_RATE_NSS GENMASK(10,9)
#define MT_TX_RATE_MODE GENMASK(8,6)
#define MT_TX_RATE_IDX GENMASK(5,0)
enum { MT_PHY_TYPE_CCK, MT_PHY_TYPE_OFDM, MT_PHY_TYPE_HT,
       MT_PHY_TYPE_HT_GF, MT_PHY_TYPE_VHT };
enum { NL80211_BAND_2GHZ, NL80211_BAND_5GHZ };
struct ieee80211_tx_rate { int idx, count; u32 flags; };
struct ieee80211_tx_info { u32 flags; struct { struct ieee80211_tx_rate rates[4]; } control; };
struct ieee80211_sta_rates { struct ieee80211_tx_rate rate[4]; };
struct ieee80211_sta { struct ieee80211_sta_rates *rates; };
struct ieee80211_rate { u16 hw_value, hw_value_short; };
struct band { struct ieee80211_rate *bitrates; };
struct wiphy { struct band *bands[2]; };
struct hw { struct wiphy *wiphy; };
struct mt76_phy { struct hw *hw; };
struct mt7615_dev { struct mt76_phy mphy; };
struct sk_buff { void *data; struct ieee80211_tx_info info; };
struct mt76_tx_info { struct sk_buff *skb; };
#define IEEE80211_SKB_CB(skb) (&(skb)->info)
static u8 ieee80211_rate_get_vht_mcs(const struct ieee80211_tx_rate *r) { return r->idx & 15; }
static u8 ieee80211_rate_get_vht_nss(const struct ieee80211_tx_rate *r) { return (r->idx >> 4) + 1; }
struct ieee80211_hdr { u16 frame_control; u8 addr1[6]; };
static bool ieee80211_is_data_present(u16 fc) { return (fc & 0x4c) == 8; }
static bool ieee80211_is_data_qos(u16 fc) { return (fc & 0x8c) == 0x88; }
static bool is_multicast_ether_addr(const u8 *a) { return a[0] & 1; }
'''
        source = (DRIVER / 'mt7663s_w103d.c').read_text(encoding='utf-8')
        actual = '\n'.join(function('mt7663s_w103d.c', name) for name in (
            'mt7663s_w103d_tx_rate_val', 'mt7663s_w103d_fix_5g_tx_rate'))
        prepare = function('mt7663s_w103d.c', 'mt7663s_w103d_tx_prepare_skb')
        classification = re.search(r'bool is_data = .*?\n\tu32 sdio_len;', prepare, re.S).group().rsplit('\n', 1)[0]
        classifier = '\nstatic unsigned classify(struct ieee80211_hdr *hdr) {\n' + classification + '\nreturn is_data | (firmware_ba << 1);\n}\n'
        checks = r'''
int main(void) {
    struct ieee80211_rate legacy[8] = {{0x10b, 0x10b}}; /* OFDM 6 Mbps */
    struct band band = {legacy}; struct wiphy wiphy = {{NULL, &band}};
    struct hw hw = {&wiphy}; struct mt7615_dev dev = {{&hw}};
    struct ieee80211_sta_rates table = {.rate = {{0x19, 2,
        IEEE80211_TX_RC_VHT_MCS | IEEE80211_TX_RC_80_MHZ_WIDTH | IEEE80211_TX_RC_SHORT_GI}}};
    struct ieee80211_sta sta = {&table};
    u32 txwi[8] = {0}; struct sk_buff skb = {.data = txwi};
    struct mt76_tx_info tx = {&skb};
    struct ieee80211_hdr hdr = {.frame_control = 0x0188};
    assert(classify(&hdr) == 3); /* unicast QoS payload can aggregate */
    hdr.addr1[0] = 1; assert(classify(&hdr) == 1);
    hdr.addr1[0] = 0;
    const u16 non_payload[] = {0x01c8, 0x0148, 0x00b0, 0x00d0};
    for (unsigned i = 0; i < sizeof(non_payload)/sizeof(non_payload[0]); i++) {
        hdr.frame_control = non_payload[i]; assert(classify(&hdr) == 0);
    }
    hdr.frame_control = 0x0108; assert(classify(&hdr) == 1);

    skb.info.flags = IEEE80211_TX_CTL_STBC | IEEE80211_TX_CTL_LDPC;
    txwi[2] = MT_TXD2_BA_DISABLE;
    mt7663s_w103d_fix_5g_tx_rate(&dev, &sta, &tx, true, true);
    u32 rate = FIELD_GET(MT_TXD6_TX_RATE, txwi[6]);
    assert(FIELD_GET(MT_TX_RATE_MODE, rate) == MT_PHY_TYPE_VHT);
    assert(FIELD_GET(MT_TX_RATE_NSS, rate) == 1 && FIELD_GET(MT_TX_RATE_IDX, rate) == 9);
    assert(FIELD_GET(MT_TXD6_BW, txwi[6]) == 2 && (txwi[6] & MT_TXD6_SGI));
    assert(txwi[6] & MT_TXD6_LDPC);
    assert(!(txwi[2] & MT_TXD2_BA_DISABLE));
    assert(FIELD_GET(MT_TXD3_REM_TX_COUNT, txwi[3]) == 2);

    /* A robust basic-rate request wins even over an explicit rate probe. */
    for (int kind = 0; kind < 3; kind++) {
        memset(txwi, 0, sizeof(txwi));
        skb.info.flags = IEEE80211_TX_CTL_STBC | IEEE80211_TX_CTL_LDPC;
        if (kind == 0) skb.info.flags |= IEEE80211_TX_CTL_USE_MINRATE | IEEE80211_TX_CTL_RATE_CTRL_PROBE;
        if (kind == 2) sta.rates = NULL; /* no published data-rate table */
        mt7663s_w103d_fix_5g_tx_rate(&dev, &sta, &tx, kind != 1, true);
        rate = FIELD_GET(MT_TXD6_TX_RATE, txwi[6]);
        assert(rate == ((MT_PHY_TYPE_OFDM << 6) | 11));
        assert(txwi[2] & MT_TXD2_FIX_RATE);
        assert(txwi[2] & MT_TXD2_BA_DISABLE);
        assert(!(txwi[6] & (MT_TXD6_SGI | MT_TXD6_LDPC | MT_TXD6_BW)));
        assert(FIELD_GET(MT_TXD3_REM_TX_COUNT, txwi[3]) == 8);
    }
    /* Ordinary minstrel sampling still uses its requested VHT rate. */
    sta.rates = &table;
    skb.info.flags = IEEE80211_TX_CTL_RATE_CTRL_PROBE;
    skb.info.control.rates[0] = (struct ieee80211_tx_rate){3, 3, IEEE80211_TX_RC_VHT_MCS};
    mt7663s_w103d_fix_5g_tx_rate(&dev, &sta, &tx, true, true);
    rate = FIELD_GET(MT_TXD6_TX_RATE, txwi[6]);
    assert(FIELD_GET(MT_TX_RATE_MODE, rate) == MT_PHY_TYPE_VHT);
    assert(FIELD_GET(MT_TX_RATE_NSS, rate) == 0 && FIELD_GET(MT_TX_RATE_IDX, rate) == 3);
    assert(FIELD_GET(MT_TXD3_REM_TX_COUNT, txwi[3]) == 3);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='w103d-connection-rate-') as tmp:
            c = Path(tmp) / 'test.c'
            exe = Path(tmp) / 'test'
            c.write_text(prelude + actual + classifier + checks)
            subprocess.run(['gcc', '-Wall', '-Werror', '-Wno-unused-parameter',
                            '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                            '-no-pie', '-g', str(c), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    unittest.main()
