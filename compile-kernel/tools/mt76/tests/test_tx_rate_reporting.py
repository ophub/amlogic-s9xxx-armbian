"""Run the actual mac80211 TX rate-reporting block from a patched kernel tree."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

KERNEL = Path(sys.argv.pop(1)) if len(sys.argv) > 1 else None


@unittest.skipUnless(os.name == 'posix' and KERNEL, 'requires Linux GCC and a patched kernel tree')
class TxRateReporting(unittest.TestCase):
    def test_null_frames_and_real_low_rate_payload(self):
        source = (KERNEL / 'net/mac80211/tx.c').read_text()
        start = source.index('\tif (txrc.reported_rate.idx < 0)')
        end = source.index('\n\tif (ratetbl)', start)
        actual = source[start:end]
        prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
struct rate { int idx; unsigned flags; };
struct sta { struct { struct { struct rate last_rate; } tx_stats; } deflink; };
struct tx { struct sta *sta; struct rate rate; };
struct header { uint16_t frame_control; };
static bool ieee80211_is_data_present(uint16_t fc) { return (fc & 0x4c) == 8; }
static void report(struct tx *tx, bool encap, uint16_t fc, struct rate selected) {
    struct { struct rate reported_rate; } txrc = {selected};
    struct header frame = {fc}; struct header *hdr = &frame;
'''
        checks = r'''
}
int main(void) {
    struct sta sta = {0}; struct tx tx = {&sta, {0, 0}};
    const uint16_t non_payload[] = {0x0148, 0x01c8, 0x00b0, 0x00d0, 0x0080};
    const uint16_t payload[] = {0x0108, 0x0188};
    for (unsigned i = 0; i < sizeof(non_payload)/sizeof(non_payload[0]); i++) {
        sta.deflink.tx_stats.last_rate = (struct rate){0x19, 1};
        report(&tx, false, non_payload[i], (struct rate){-1, 0});
        assert(sta.deflink.tx_stats.last_rate.idx == 0x19);
        assert(sta.deflink.tx_stats.last_rate.flags == 1);
        report(&tx, false, non_payload[i], (struct rate){0, 0});
        assert(sta.deflink.tx_stats.last_rate.idx == 0x19);
    }
    for (unsigned i = 0; i < sizeof(payload)/sizeof(payload[0]); i++) {
        /* A real payload sent at a legacy rate must still be reported. */
        sta.deflink.tx_stats.last_rate = (struct rate){0x19, 1};
        report(&tx, false, payload[i], (struct rate){-1, 0});
        assert(sta.deflink.tx_stats.last_rate.idx == 0);
        assert(sta.deflink.tx_stats.last_rate.flags == 0);
        report(&tx, false, payload[i], (struct rate){7, 1});
        assert(sta.deflink.tx_stats.last_rate.idx == 7);
    }
    report(&tx, true, 0x0148, (struct rate){9, 1});
    assert(sta.deflink.tx_stats.last_rate.idx == 9);
    tx.sta = NULL;
    report(&tx, false, 0x0188, (struct rate){7, 1});
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='w103d-tx-rate-reporting-') as tmp:
            c = Path(tmp) / 'test.c'
            exe = Path(tmp) / 'test'
            c.write_text(prelude + actual + checks)
            subprocess.run(['gcc', '-Wall', '-Werror', '-fsanitize=address,undefined',
                            '-fno-omit-frame-pointer', '-no-pie', '-g', str(c), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    unittest.main()
