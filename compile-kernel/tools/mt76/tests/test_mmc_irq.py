"""Compile the actual Meson hard-IRQ handler against a bounded fake host."""
from pathlib import Path
import subprocess, tempfile, sys
if len(sys.argv) != 2:
    raise SystemExit('Usage: test_mmc_irq.py <patched-kernel-source-directory>')
source=(Path(sys.argv[1])/'drivers/mmc/host/meson-gx-mmc.c').read_text()
start=source.index('static irqreturn_t meson_mmc_irq(')
end=source.index('\nstatic int meson_mmc_wait_desc_stop',start)
handler=source[start:end]
prefix=r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
typedef uint32_t u32;
typedef int irqreturn_t;
#define IRQ_NONE 0
#define IRQ_HANDLED 1
#define IRQ_WAKE_THREAD 2
#define MMC_CAP_SDIO_IRQ 1
#define IRQ_SDIO 0x100
#define IRQ_CRC_ERR 0x20
#define IRQ_TIMEOUTS 0x40
#define IRQ_END_OF_CHAIN 0x1
#define IRQ_RESP_STATUS 0x2
#define IRQ_EN_MASK 0x7f
#define SD_EMMC_STATUS 0
#define SD_EMMC_START 4
#define START_DESC_BUSY 1
#define SD_IO_RW_DIRECT 52
#define SD_IO_RW_EXTENDED 53
#define WARN_ON(x) (x)
#define dev_dbg(...) ((void)0)
#define spin_lock(x) ((void)0)
#define spin_unlock(x) ((void)0)
struct mmc_data { unsigned flags, blksz, blocks, bytes_xfered; int error; bool bounce; };
struct mmc_request { int unused; };
struct mmc_command { int opcode,error; struct mmc_data *data; struct mmc_request *mrq; bool next; };
struct mmc_host { unsigned caps; };
struct meson_host { struct mmc_host *mmc; struct mmc_command *cmd; unsigned char *regs; bool sdio_irq_fastpath; void *dev; int lock; };
static int completed,signaled;
static u32 registers[2];
static u32 readl(void *p) { return *(u32 *)p; }
static void writel(u32 v, void *p) { *(u32 *)p=v; }
static void __meson_mmc_enable_sdio_irq(struct mmc_host *h,int enable) {(void)h;(void)enable;}
static void sdio_signal_irq(struct mmc_host *h) {(void)h;signaled++;}
static void meson_mmc_read_resp(struct mmc_host *h,struct mmc_command *c) {(void)h;(void)c;}
static bool meson_mmc_bounce_buf_read(struct mmc_data *d) {return d && d->bounce;}
static void *meson_mmc_get_next_command(struct mmc_command *c) {return c->next ? c : NULL;}
static void meson_mmc_request_done(struct mmc_host *h,struct mmc_request *r) {(void)h;assert(r);completed++;}
'''
suffix=r'''
int main(void) {
    struct mmc_host mmc={.caps=MMC_CAP_SDIO_IRQ};
    struct mmc_request request={0};
    unsigned cases=0;
    for (unsigned enabled=0;enabled<2;enabled++)
    for (unsigned opcode=17;opcode<=53;opcode++)
    for (unsigned mode=0;mode<4;mode++)
    for (unsigned error=0;error<3;error++)
    for (unsigned chain=0;chain<2;chain++)
    for (unsigned sdio=0;sdio<2;sdio++) {
        struct mmc_data data={.blksz=112,.blocks=2,.bounce=mode==2,.error=mode==3};
        struct mmc_command cmd={.opcode=opcode,.mrq=&request,.data=mode?&data:NULL,.next=chain};
        struct meson_host host={.mmc=&mmc,.cmd=&cmd,.regs=(void *)registers,.sdio_irq_fastpath=enabled};
        completed=signaled=0;
        registers[0]=IRQ_END_OF_CHAIN|(sdio?IRQ_SDIO:0)|(error==1?IRQ_CRC_ERR:0)|(error==2?IRQ_TIMEOUTS:0);
        registers[1]=START_DESC_BUSY;
        int ret=meson_mmc_irq(19,&host);
        bool fast=enabled&&(opcode==52||opcode==53)&&mode!=2&&mode!=3&&!error&&!chain;
        assert(ret==(fast?IRQ_HANDLED:IRQ_WAKE_THREAD));
        assert(completed==(int)fast);
        assert(signaled==(int)sdio);
        if(error) assert(!(registers[1]&START_DESC_BUSY));
        cases++;
    }
    struct meson_host host={.mmc=&mmc,.regs=(void *)registers,.sdio_irq_fastpath=true};
    registers[0]=IRQ_SDIO;completed=signaled=0;
    assert(meson_mmc_irq(19,&host)==IRQ_HANDLED&&completed==0&&signaled==1);
    registers[0]=0;
    assert(meson_mmc_irq(19,&host)==IRQ_NONE&&completed==0);
    printf("%u IRQ completion/error/fallback combinations and SDIO-only/no-IRQ cases passed\n",cases);
}
'''
with tempfile.TemporaryDirectory(prefix='w103d-mmc-test-') as temp:
    path=Path(temp)
    (path/'test.c').write_text(prefix+handler+suffix)
    subprocess.run(['gcc','-std=gnu11','-Wall','-Wextra','-Wno-unused-parameter','-Werror','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie','-g',str(path/'test.c'),'-o',str(path/'test')],check=True)
    subprocess.run([str(path/'test')],check=True)
