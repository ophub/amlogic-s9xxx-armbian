"""Execute the actual card coordinator with pthread-backed kernel primitives."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

COMBO = Path(__file__).resolve().parents[1] / 'combo'


@unittest.skipUnless(os.name == 'posix', 'requires Linux GCC and pthreads')
class NativeCombo(unittest.TestCase):
    def test_readiness_lifetime_and_transition_serialization(self):
        prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#define IS_ENABLED(x) 1
#define EPROBE_DEFER 517
#define ERR_PTR(x) ((void *)(intptr_t)(x))
#define PTR_ERR(x) ((int)(intptr_t)(x))
#define IS_ERR(x) ((uintptr_t)(x) >= (uintptr_t)-4095)
#define EXPORT_SYMBOL_GPL(x)
#define GFP_KERNEL 0
#define HZ 10
#define kzalloc(n, f) calloc(1, n)
#define kfree(x) free(x)
#define DL_FLAG_AUTOPROBE_CONSUMER 32
#define dev_info(...) ((void)0)
struct hci_dev { void *driver_data; };
static void *hci_get_drvdata(struct hci_dev *h) { return h->driver_data; }
struct mutex { pthread_mutex_t p; };
#define DEFINE_MUTEX(n) struct mutex n = {PTHREAD_MUTEX_INITIALIZER}
static void mutex_init(struct mutex *m) { assert(!pthread_mutex_init(&m->p, NULL)); }
static void mutex_lock(struct mutex *m) { assert(!pthread_mutex_lock(&m->p)); }
static void mutex_unlock(struct mutex *m) { assert(!pthread_mutex_unlock(&m->p)); }
struct completion { pthread_mutex_t lock; pthread_cond_t cond; bool done; };
static void init_completion(struct completion *c) {
    pthread_mutex_init(&c->lock, NULL); pthread_cond_init(&c->cond, NULL); c->done=false;
}
static void complete_all(struct completion *c) {
    pthread_mutex_lock(&c->lock); c->done=true; pthread_cond_broadcast(&c->cond); pthread_mutex_unlock(&c->lock);
}
static int wait_for_completion_timeout(struct completion *c, int ms) {
    struct timespec until; clock_gettime(CLOCK_REALTIME, &until);
    until.tv_nsec += ms*1000000L; until.tv_sec += until.tv_nsec/1000000000L; until.tv_nsec %= 1000000000L;
    pthread_mutex_lock(&c->lock);
    while (!c->done) { if (pthread_cond_timedwait(&c->cond, &c->lock, &until)==ETIMEDOUT) break; }
    int done=c->done; pthread_mutex_unlock(&c->lock); return done;
}
typedef atomic_int refcount_t;
static void refcount_set(refcount_t *r, int n) { atomic_store(r,n); }
static void refcount_inc(refcount_t *r) { atomic_fetch_add(r,1); }
static bool refcount_dec_and_test(refcount_t *r) { return atomic_fetch_sub(r,1)==1; }
struct list_head { struct list_head *next, *prev; };
#define LIST_HEAD(n) struct list_head n = {&n, &n}
#define container_of(p,t,m) ((t *)((char *)(p)-offsetof(t,m)))
#define list_for_each_entry(p,h,m) for (struct list_head *_n=(h)->next; _n!=(h) && ((p)=container_of(_n,__typeof__(*(p)),m),1); _n=_n->next)
static void list_add_tail(struct list_head *n, struct list_head *h) { n->prev=h->prev; n->next=h; h->prev->next=n; h->prev=n; }
static void list_del(struct list_head *n) { n->prev->next=n->next; n->next->prev=n->prev; }
struct device { int refs; bool bound; void (*cleanup)(void *); void *data; };
struct sdio_func { struct device dev; void *card; };
struct device_link { int unused; };
struct bt_data { unsigned chipid; };
struct mt7663_combo;
struct btmtksdio_dev { struct device *dev; struct sdio_func *func; const struct bt_data *data; struct mt7663_combo *combo; };
static bool w103d=true, link_fail=false, action_fail=false;
static struct device_link link_object;
static bool of_machine_is_compatible(const char *s) { return w103d; }
static void get_device(struct device *d) { ++d->refs; }
static void put_device(struct device *d) { assert(d->refs>0); --d->refs; }
static bool device_is_bound(struct device *d) { return d->bound; }
static struct device_link *device_link_add(struct device *c, struct device *s, int flags) {
    assert(c!=s && flags==DL_FLAG_AUTOPROBE_CONSUMER); return link_fail ? NULL : &link_object;
}
static int devm_add_action_or_reset(struct device *d, void (*a)(void *), void *data) {
    if (action_fail) { a(data); return -ENOMEM; } d->cleanup=a; d->data=data; return 0;
}
'''
        checks = r'''
static atomic_bool started, finished;
static void *waiter(void *data) {
    atomic_store(&started,true);
    assert(mt7663_combo_begin(data)==0);
    atomic_store(&finished,true);
    mt7663_combo_end(data); return NULL;
}
static void ready(struct mt7663_combo *c, int ret) {
    mutex_lock(&c->transition); c->setup_status=ret; complete_all(&c->setup_done); mutex_unlock(&c->transition);
}
int main(void) {
    int card1, card2;
    struct sdio_func bt1={.card=&card1}, wifi1={.card=&card1};
    struct sdio_func bt2={.card=&card2}, wifi2={.card=&card2};
    const struct bt_data data={.chipid=0x7663};
    struct btmtksdio_dev b1={.dev=&bt1.dev,.func=&bt1,.data=&data};
    struct btmtksdio_dev b2={.dev=&bt2.dev,.func=&bt2,.data=&data};
    struct hci_dev h1={.driver_data=&b1};
    w103d=false;
    assert(mt7663_combo_get(&wifi1)==NULL && mt7663_combo_begin(NULL)==0);
    mt7663_combo_end(NULL); mt7663_combo_put(NULL); w103d=true;
    assert(PTR_ERR(mt7663_combo_get(&wifi1))==-EPROBE_DEFER);
    action_fail=true;
    assert(btmtksdio_combo_register(&b1)==-ENOMEM && bt1.dev.refs==0);
    assert(PTR_ERR(mt7663_combo_get(&wifi1))==-EPROBE_DEFER);
    action_fail=false;
    assert(!btmtksdio_combo_register(&b1));
    assert(PTR_ERR(mt7663_combo_get(&wifi1))==-EPROBE_DEFER);
    assert(atomic_load(&b1.combo->refs)==1);
    bt1.dev.bound=true;
    link_fail=true;
    assert(PTR_ERR(mt7663_combo_get(&wifi1))==-EINVAL);
    assert(atomic_load(&b1.combo->refs)==1); link_fail=false;
    struct mt7663_combo *c1=mt7663_combo_get(&wifi1);
    assert(c1==b1.combo && atomic_load(&c1->refs)==2);
    assert(mt7663_combo_begin(c1)==-ETIMEDOUT);
    ready(c1,-EIO); assert(mt7663_combo_begin(c1)==-EIO);
    assert(!btmtksdio_combo_post_init(&h1));
    assert(!mt7663_combo_begin(c1));
    pthread_t thread; assert(!pthread_create(&thread,NULL,waiter,c1));
    while (!atomic_load(&started)) sched_yield();
    usleep(10000); assert(!atomic_load(&finished));
    /* A different card must not be blocked by card1's transition. */
    assert(!btmtksdio_combo_register(&b2)); bt2.dev.bound=true;
    struct mt7663_combo *c2=mt7663_combo_get(&wifi2);
    ready(c2,0); assert(!mt7663_combo_begin(c2)); mt7663_combo_end(c2);
    mt7663_combo_end(c1); assert(!pthread_join(thread,NULL)); assert(atomic_load(&finished));
    /* Provider removal wakes waiters; consumer refs survive devres cleanup. */
    bt1.dev.cleanup(bt1.dev.data);
    assert(PTR_ERR(mt7663_combo_get(&wifi1))==-EPROBE_DEFER);
    assert(bt1.dev.refs==1 && mt7663_combo_begin(c1)==-ENODEV);
    mt7663_combo_put(c1); assert(bt1.dev.refs==0);
    mt7663_combo_put(c2); bt2.dev.cleanup(bt2.dev.data); assert(bt2.dev.refs==0);
    /* A fresh Bluetooth bind must not inherit the previous completion. */
    assert(!btmtksdio_combo_register(&b1)); c1=mt7663_combo_get(&wifi1);
    assert(mt7663_combo_begin(c1)==-ETIMEDOUT);
    btmtksdio_combo_detach(c1); assert(mt7663_combo_begin(c1)==-ENODEV);
    assert(btmtksdio_combo_post_init(&h1)==-ENODEV);
    assert(mt7663_combo_begin(c1)==-ENODEV);
    mt7663_combo_put(c1); bt1.dev.cleanup(bt1.dev.data); assert(bt1.dev.refs==0);
    puts("PASS: readiness, errors, card isolation, serialization, refs, fresh bind");
}
'''
        with tempfile.TemporaryDirectory(prefix='w103d-combo-') as tmp:
            root = Path(tmp)
            (root / 'linux').mkdir()
            for name in ['completion.h', 'refcount.h', 'kconfig.h']:
                (root / 'linux' / name).write_text('')
            (root / 'linux/mt7663-combo.h').write_text((COMBO / 'mt7663-combo.h').read_text())
            source = root / 'combo.c'
            source.write_text(prelude + (COMBO / 'btmtksdio-w103d.h').read_text() + checks)
            binary = root / 'combo'
            subprocess.run(['gcc', '-std=gnu11', '-g', '-O1', '-pthread',
                            '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                            '-fno-pie', '-no-pie', '-I', str(root), str(source),
                            '-o', str(binary)], check=True, capture_output=True, text=True)
            result = subprocess.run([str(binary)], check=True, capture_output=True, text=True, timeout=10)
            self.assertIn('PASS:', result.stdout)


if __name__ == '__main__':
    unittest.main()
