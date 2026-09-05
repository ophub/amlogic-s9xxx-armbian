"""Execute the driver's RX length/parser/transaction functions with a fake SDIO FIFO.

This tests real C control flow, not a Python model and not hardware timing.
"""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

DRIVER = Path(__file__).resolve().parents[1] / 'mt7663s'


def function(filename, name):
    source = (DRIVER / filename).read_text()
    pos = source.index(name + '(')
    start = source.rfind('\n', 0, pos) + 1
    if start == pos:
        start = source.rfind('\n', 0, start - 1) + 1
    brace = source.index('{', pos)
    end, depth = brace + 1, 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end] + '\n'


@unittest.skipUnless(os.name == 'posix', 'requires Linux gcc')
class NativeRxEnhance(unittest.TestCase):
    def test_fifo_boundaries_and_snapshot_lifecycle(self):
        prelude = r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t __le32;
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#define GENMASK(h,l) ((~0u >> (31-(h))) & (~0u << (l)))
#define FIELD_GET(m,v) (((v)&(m)) >> __builtin_ctz(m))
#define le32_get_bits(v,m) FIELD_GET(m,v)
#define TXQ_CNT_L 0xffffu
#define TXQ_CNT_H 0xffff0000u
#define round_up(n,a) (((n)+(a)-1)&~((a)-1))
#define roundup(n,a) ((((n)+(a)-1)/(a))*(a))
#define W103D_RX_ENHANCE_SIZE 112
#define W103D_RX_ENHANCE_PAD 4
#define WHIER_RX0_DONE_INT_EN 2u
#define WHIER_RX1_DONE_INT_EN 4u
#define WHIER_D2H_SW_INT 0xffffff00u
#define H2D_SW_INT_READ (1u<<16)
#define H2D_SW_INT_WRITE (1u<<17)
#define MCR_WRDR(q) (q)
#define GFP_KERNEL 0
#define unlikely(x) (x)
#define dev_err(...) (++errors)
#define dev_err_ratelimited(...) (++errors)
#define dev_warn_ratelimited(...) (++errors)
#define spin_lock_bh(x) ((void)0)
#define spin_unlock_bh(x) ((void)0)
#define BUILD_BUG_ON(x) _Static_assert(!(x), "layout")
#define trace_dev_irq(...) ((void)0)
enum mt76_rxq_id { RX0, RX1 };
struct sk_buff { int unused; };
struct page { unsigned char *memory; };
struct mt76_queue_entry { struct sk_buff *skb; };
struct mt76_queue { int head, queued, ndesc, lock; struct mt76_queue_entry entry[128]; };
struct mt76s_intr { u32 isr, *rec_mb; struct { u32 *wtqcr; } tx;
    struct { u16 *num; u16 *len[2]; } rx; };
struct mt7663s_intr { u32 isr; struct { u32 wtqcr[8]; } tx;
    struct { u16 num[2]; u16 len[2][16]; } rx; u32 rec_mb[2]; };
struct mt7663s_w103d_intr_state { struct mt7663s_intr dispatch; u16 rx_num_buf[2]; };
struct sdio_func { int cur_blksize; };
struct mt76_dev;
struct mt76_sdio { struct sdio_func *func; int net_worker;
    struct { int pse_data_quota, ple_data_quota, pse_mcu_quota; } sched;
    int (*parse_irq)(struct mt76_dev *, struct mt76s_intr *); };
struct driver { bool (*rx_check)(struct mt76_dev *, void *, int); };
struct mt76_dev { struct mt76_queue q_rx[2]; struct mt76_sdio sdio; struct driver *drv; };
static struct mt7663s_w103d_intr_state state;
static unsigned long rx_enhance_reads, rx_enhance_tx_reports, rx_enhance_pse, rx_enhance_ple, rx_enhance_errors;
static int claims, errors, reads, last_len, fault, alloc_failure, checked, saved, wire_index;
static unsigned char wire[3][65536];
static int wire_size[3], wire_port[3];
static struct mt7663s_intr initial, pending;
static struct sk_buff fake_skb;
static struct mt7663s_w103d_intr_state *mt7663s_w103d_intr_state(struct mt76_dev *d) { return &state; }
static int get_order(int len) { int n=0; while ((4096<<n)<len) n++; return n; }
static struct page *__dev_alloc_pages(int flags, int order) {
    if (alloc_failure) return NULL;
    struct page *p=malloc(sizeof(*p)); p->memory=calloc(1,4096<<order); return p;
}
static void *page_address(struct page *p) { return p->memory; }
static void put_page(struct page *p) { free(p->memory); free(p); }
static u32 get_unaligned_le32(const void *p) { u32 x; memcpy(&x,p,4); return x; }
static void sdio_claim_host(struct sdio_func *f) { claims++; }
static void sdio_release_host(struct sdio_func *f) { assert(claims>0); claims--; }
static int sdio_readsb(struct sdio_func *f, void *dst, int port, int len) {
    assert(claims>0); reads++; last_len=len;
    if (fault) return -EIO;
    assert(wire_index<3 && port==wire_port[wire_index]);
    assert(len==wire_size[wire_index]);
    memcpy(dst,wire[wire_index++],len); return 0;
}
static void mt76_worker_schedule(void *worker) { }
static struct sk_buff *mt76s_build_rx_skb(void *data, int len, int span) { return &fake_skb; }
static bool check_packet(struct mt76_dev *d, void *data, int len) {
    checked++; return !(((u8 *)data)[4]); /* mark TXS: consumed, not enqueued */
}
'''
        code = prelude
        for name in ('mt7663s_w103d_fill_intr', 'mt7663s_w103d_rx_enhance'):
            code += function('mt7663s_w103d.c', name)
        code += r'''
static int parse(struct mt76_dev *d, struct mt76s_intr *intr) {
    assert(claims>0);
    state.dispatch=saved ? pending : initial;
    saved=0; memset(&initial,0,sizeof(initial));
    mt7663s_w103d_fill_intr(intr,&state.dispatch,&state); return 0;
}
static void mt7663s_w103d_save_intr(struct mt76_dev *d, struct mt76s_intr *intr) {
    assert(claims>0); pending=state.dispatch; pending.isr=intr->isr;
    saved=!!(pending.isr || pending.rx.num[0] || pending.rx.num[1]);
    for(int i=0;i<8;i++) assert(pending.tx.wtqcr[i]==0);
}
'''
        for name in ('mt76s_refill_sched_quota', 'mt76s_rx_run_queue',
                     'mt76s_consume_intr', 'mt76s_rx_handler'):
            code += function('mt7663s_txrx.c', name)
        code += r'''
static struct sdio_func func={512};
static struct driver drv={check_packet};
static struct mt76_dev dev;
static void reset(void) {
    memset(&dev,0,sizeof(dev)); memset(&state,0,sizeof(state));
    memset(&initial,0,sizeof(initial)); memset(&pending,0,sizeof(pending));
    memset(wire,0,sizeof(wire));
    claims=errors=reads=last_len=fault=alloc_failure=checked=saved=wire_index=0;
    rx_enhance_reads=rx_enhance_tx_reports=rx_enhance_pse=rx_enhance_ple=rx_enhance_errors=0;
    dev.sdio.func=&func; dev.sdio.parse_irq=parse; dev.drv=&drv;
    dev.q_rx[0].ndesc=dev.q_rx[1].ndesc=128;
}
static void packet_view(struct mt7663s_intr *s, int port, int count, int size) {
    s->isr|=port?4:2; s->rx.num[port]=count;
    for(int i=0;i<count;i++) s->rx.len[port][i]=size;
}
static int prepare(int index,int port,int count,int size,const struct mt7663s_intr *tail) {
    int span=round_up(size+4,4), d=count*span, l=d+116;
    wire_size[index]=l>512?roundup(l,512):l; wire_port[index]=port;
    for(int i=0;i<count;i++) { u32 len=size; memcpy(wire[index]+i*span,&len,4); }
    memcpy(wire[index]+d+4,tail,sizeof(*tail)); return d;
}
int main(void) {
    struct mt7663s_intr tail={0};
    int sizes[]={4,96,388,392,396,1400,1500,4092};
    for(unsigned i=0;i<ARRAY_SIZE(sizes);i++) {
        reset(); packet_view(&initial,0,1,sizes[i]); prepare(0,0,1,sizes[i],&tail);
        assert(mt76s_rx_handler(&dev)==1); assert(reads==1 && checked==1 && claims==0);
        assert(dev.q_rx[0].queued==1 && rx_enhance_reads==1 && !saved);
    }
    reset(); packet_view(&initial,0,16,100); prepare(0,0,16,100,&tail);
    for(int i=0;i<16;i+=2) wire[0][i*104+4]=1;
    assert(mt76s_rx_handler(&dev)==16); assert(checked==16 && dev.q_rx[0].queued==8);
    reset(); initial.rx.num[0]=17;
    assert(mt76s_rx_handler(&dev)==-EPROTO && reads==0 && claims==0);
    reset(); packet_view(&initial,0,1,100); alloc_failure=1;
    assert(mt76s_rx_handler(&dev)==-ENOMEM && reads==0 && saved && claims==0);
    reset(); packet_view(&initial,0,1,100); fault=1;
    assert(mt76s_rx_handler(&dev)==-EIO && !rx_enhance_reads && claims==0);
    reset(); packet_view(&initial,0,1,100); int d=prepare(0,0,1,100,&tail); wire[0][d+2]=1;
    assert(mt76s_rx_handler(&dev)==-EPROTO && !rx_enhance_reads && claims==0);
    reset(); tail.rx.num[1]=17; packet_view(&initial,0,1,100); prepare(0,0,1,100,&tail);
    assert(mt76s_rx_handler(&dev)==-EPROTO && rx_enhance_errors==1 && claims==0);
    memset(&tail,0,sizeof(tail));
    /* E0 old credits, E1 RX0 tail updates RX1, E2 RX1 tail queues RX0. */
    reset(); initial.tx.wtqcr[0]=10; initial.tx.wtqcr[2]=2u<<16;
    packet_view(&initial,0,1,100);
    struct mt7663s_intr e1={0},e2={0};
    e1.tx.wtqcr[0]=20; e1.tx.wtqcr[2]=3u<<16; packet_view(&e1,1,1,200);
    e2.tx.wtqcr[0]=30; e2.tx.wtqcr[2]=4u<<16; packet_view(&e2,0,1,300);
    prepare(0,0,1,100,&e1); prepare(1,1,1,200,&e2); prepare(2,0,1,300,&tail);
    assert(mt76s_rx_handler(&dev)==5); assert(reads==2 && saved && pending.rx.num[0]==1);
    assert(dev.sdio.sched.pse_data_quota==60 && dev.sdio.sched.ple_data_quota==9);
    assert(mt76s_rx_handler(&dev)==1); assert(reads==3 && !saved && claims==0);
    assert(dev.sdio.sched.pse_data_quota==60 && dev.sdio.sched.ple_data_quota==9);
    assert(mt76s_rx_handler(&dev)==0);
    assert(rx_enhance_pse==50 && rx_enhance_ple==7 && rx_enhance_tx_reports==2);
    puts("native RX enhance: boundaries, 16/TXS, failures, E0/E1/E2, exactly-once passed");
}
'''
        with tempfile.TemporaryDirectory(prefix='w103d-rx-native-') as temp:
            path = Path(temp)
            (path / 'test.c').write_text(code)
            subprocess.run(['gcc', '-std=gnu11', '-Wall', '-Werror',
                            '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True, timeout=20)


if __name__ == '__main__':
    unittest.main()
