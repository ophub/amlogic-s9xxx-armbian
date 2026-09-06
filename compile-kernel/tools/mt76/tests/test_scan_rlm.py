"""Check the actual gen4m home-channel encoder and MCU submission under ASan/UBSan."""
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
from test_rx_enhance import DRIVER, function


@unittest.skipUnless(os.name == 'posix', 'requires Linux GCC')
class NativeScanRlm(unittest.TestCase):
    def test_vendor_wire_layout_and_channel_placement(self):
        prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint16_t __le16;
#define __packed __attribute__((packed))
#define cpu_to_le16(x) ((u16)(x))
#define hweight8(x) __builtin_popcount((u8)(x))
#define max_t(t,a,b) ((t)(a) > (t)(b) ? (t)(a) : (t)(b))
#define BUILD_BUG_ON(x) _Static_assert(!(x), #x)
#define IEEE80211_HT_OP_MODE_PROTECTION 3
#define IEEE80211_HT_PARAM_CHAN_WIDTH_ANY 4
#define MCU_CMD_VENDOR_SET_BSS_RLM_PARAM 0x19
#define MCU_CMD_VENDOR_SET(x) (0x10000 | (x))
enum nl80211_band { NL80211_BAND_2GHZ, NL80211_BAND_5GHZ };
enum { NL80211_CHAN_WIDTH_20, NL80211_CHAN_WIDTH_40, NL80211_CHAN_WIDTH_80 };
struct ieee80211_channel { enum nl80211_band band; int hw_value, center_freq; };
struct cfg80211_chan_def { struct ieee80211_channel *chan; int width, center_freq1; };
struct mt76_phy { struct cfg80211_chan_def chandef; u8 antenna_mask; };
struct mt7615_vif { struct { u8 idx; } mt76; };
struct ieee80211_vif {
    struct { bool assoc; } cfg;
    struct { bool use_cts_prot, use_short_preamble, use_short_slot; u16 ht_operation_mode; u32 basic_rates; } bss_conf;
    struct mt7615_vif drv_priv[1];
};
struct mt76_dev { int unused; };
struct mt7615_dev { struct mt76_phy mphy; struct mt76_dev mt76; };
static int ieee80211_frequency_to_channel(int f) { return (f - 5000) / 5; }
static u8 sent[22];
static unsigned calls;
static int send_result;
static int mt76_mcu_send_msg(struct mt76_dev *d, int cmd, const void *data, int len, bool wait) {
    assert(cmd == 0x10019 && len == 22 && !wait);
    memcpy(sent, data, len); calls++; return send_result;
}
'''
        source = (DRIVER / 'mt7663s_w103d.c').read_text(encoding='utf-8')
        wire = re.search(r'struct mt7663s_w103d_bss_rlm \{.*?\} __packed;', source, re.S).group()
        actual = '\n'.join(function('mt7663s_w103d.c', name) for name in (
            'mt7663s_w103d_vendor_rate_set', 'mt7663s_w103d_fill_bss_rlm',
            'mt7663s_w103d_sync_bss_rlm'))
        checks = r'''
int main(void) {
    struct ieee80211_channel ch = {NL80211_BAND_5GHZ, 40, 5200};
    struct mt7615_dev dev = {.mphy = {{&ch, NL80211_CHAN_WIDTH_80, 5210}, 3}};
    struct ieee80211_vif vif = {.cfg.assoc = true,
        .bss_conf = {true, true, true, 3, 0x15}, .drv_priv = {{{3}}}};
    const u8 expected[22] = {3,2,40,3,1,3,0,0,0,0,3,0,7,1,1,1,42,0,0x40,5,2,0};
    assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif));
    assert(calls == 1 && !memcmp(sent, expected, 22));
    /* Every primary position in two different 80 MHz blocks. */
    for (int block = 0; block < 2; block++) {
        int first = block ? 149 : 36;
        dev.mphy.chandef.center_freq1 = 5000 + (first + 6) * 5;
        for (int i = 0; i < 4; i++) {
            ch.hw_value = first + 4*i; ch.center_freq = 5000 + ch.hw_value*5;
            assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif));
            assert(sent[2] == ch.hw_value && sent[3] == (i%2 ? 3 : 1));
            assert(sent[15] == 1 && sent[16] == first+6 && sent[17] == 0);
        }
    }
    /* HT40 above/below on 2.4 GHz, and clearing old VHT fields for HT20. */
    ch = (struct ieee80211_channel){NL80211_BAND_2GHZ, 6, 2437};
    dev.mphy.chandef.width = NL80211_CHAN_WIDTH_40;
    vif.bss_conf.basic_rates = 0x15f;
    for (int side = -1; side <= 1; side += 2) {
        dev.mphy.chandef.center_freq1 = ch.center_freq + 10*side;
        assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif));
        assert(sent[1] == 1 && sent[2] == 6 && sent[3] == (side>0 ? 1 : 3));
        assert(sent[15] == 0 && sent[16] == 0 && sent[18] == 0x4f && sent[19] == 5);
    }
    dev.mphy.chandef.width = NL80211_CHAN_WIDTH_20;
    dev.mphy.antenna_mask = 1;
    assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif));
    assert(sent[3] == 0 && sent[12] == 0 && sent[20] == 1 && sent[21] == 0);
    send_result = -5;
    assert(mt7663s_w103d_sync_bss_rlm(&dev, &vif) == -5);
    unsigned saved_calls = calls;
    vif.cfg.assoc = false;
    assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif) && calls == saved_calls);
    vif.cfg.assoc = true; dev.mphy.chandef.chan = NULL;
    assert(!mt7663s_w103d_sync_bss_rlm(&dev, &vif) && calls == saved_calls);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='w103d-scan-rlm-') as tmp:
            c = Path(tmp) / 'test.c'
            exe = Path(tmp) / 'test'
            c.write_text(prelude + wire + actual + checks)
            subprocess.run(['gcc', '-Wall', '-Werror', '-Wno-unused-parameter',
                '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                '-no-pie', '-g', str(c), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    unittest.main()
