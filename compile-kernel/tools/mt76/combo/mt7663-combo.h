/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MT7663_COMBO_H
#define _LINUX_MT7663_COMBO_H

#include <linux/kconfig.h>

struct sdio_func;
struct mt7663_combo;

/* The Bluetooth supplier owns the card's WMT setup state.  A successful
 * begin holds its transition mutex; no SDIO host claim may surround it.
 */
#if IS_ENABLED(CONFIG_MT7663S_W103D_COMBO) || defined(MT7663_COMBO_PROVIDER)
struct mt7663_combo *mt7663_combo_get(struct sdio_func *func);
void mt7663_combo_put(struct mt7663_combo *combo);
int mt7663_combo_begin(struct mt7663_combo *combo);
void mt7663_combo_end(struct mt7663_combo *combo);
#else
static inline struct mt7663_combo *mt7663_combo_get(struct sdio_func *func)
{
	return NULL;
}
static inline void mt7663_combo_put(struct mt7663_combo *combo) {}
static inline int mt7663_combo_begin(struct mt7663_combo *combo) { return 0; }
static inline void mt7663_combo_end(struct mt7663_combo *combo) {}
#endif
#endif
