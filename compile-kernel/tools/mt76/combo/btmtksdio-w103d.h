/* SPDX-License-Identifier: GPL-2.0 */
/* Included by btmtksdio.c.  State is per SDIO card, never per hci index. */
#include <linux/completion.h>
#include <linux/refcount.h>
#define MT7663_COMBO_PROVIDER
#include <linux/mt7663-combo.h>

struct mt7663_combo {
	struct list_head node;
	struct sdio_func *bt_func;
	struct mutex transition;
	struct completion setup_done;
	refcount_t refs;
	int setup_status;
	bool removing;
};

static LIST_HEAD(mt7663_combos);
static DEFINE_MUTEX(mt7663_combos_lock);

static int btmtksdio_combo_post_init(struct hci_dev *hdev)
{
	struct btmtksdio_dev *bdev = hci_get_drvdata(hdev);
	struct mt7663_combo *combo = bdev->combo;
	int ret;

	if (!combo)
		return 0;
	/* setup() only downloads firmware. The following HCI initialization
	 * still reads BD_ADDR from the shared eFuse engine. Publish readiness
	 * after that sequence, before Wi-Fi starts direct eFuse register reads.
	 */
	mutex_lock(&combo->transition);
	ret = combo->removing ? -ENODEV : 0;
	combo->setup_status = ret;
	complete_all(&combo->setup_done);
	mutex_unlock(&combo->transition);
	dev_info(bdev->dev, "W103D: combo HCI initialization result=%d\n", ret);
	return ret;
}

void mt7663_combo_put(struct mt7663_combo *combo)
{
	if (combo && refcount_dec_and_test(&combo->refs)) {
		put_device(&combo->bt_func->dev);
		kfree(combo);
	}
}
EXPORT_SYMBOL_GPL(mt7663_combo_put);

struct mt7663_combo *mt7663_combo_get(struct sdio_func *func)
{
	struct mt7663_combo *combo, *found = ERR_PTR(-EPROBE_DEFER);
	struct device_link *link;

	if (!of_machine_is_compatible("zte,w103d"))
		return NULL;

	mutex_lock(&mt7663_combos_lock);
	list_for_each_entry(combo, &mt7663_combos, node) {
		if (combo->bt_func->card != func->card)
			continue;
		refcount_inc(&combo->refs);
		found = combo;
		break;
	}
	mutex_unlock(&mt7663_combos_lock);
	if (IS_ERR(found))
		return found;

	/* Managed links also order sysfs unbind and system suspend/resume.
	 * Firmware readiness is checked separately by combo_begin().
	 */
	link = device_link_add(&func->dev, &found->bt_func->dev,
			       DL_FLAG_AUTOPROBE_CONSUMER);
	if (!link) {
		mt7663_combo_put(found);
		return ERR_PTR(-EINVAL);
	}
	if (!device_is_bound(&found->bt_func->dev)) {
		mt7663_combo_put(found);
		return ERR_PTR(-EPROBE_DEFER);
	}
	return found;
}
EXPORT_SYMBOL_GPL(mt7663_combo_get);

int mt7663_combo_begin(struct mt7663_combo *combo)
{
	int ret;

	if (!combo)
		return 0;
	if (!wait_for_completion_timeout(&combo->setup_done, 20 * HZ))
		return -ETIMEDOUT;
	mutex_lock(&combo->transition);
	ret = combo->removing ? -ENODEV : combo->setup_status;
	if (ret)
		mutex_unlock(&combo->transition);
	return ret;
}
EXPORT_SYMBOL_GPL(mt7663_combo_begin);

void mt7663_combo_end(struct mt7663_combo *combo)
{
	if (combo)
		mutex_unlock(&combo->transition);
}
EXPORT_SYMBOL_GPL(mt7663_combo_end);

static void btmtksdio_combo_detach(struct mt7663_combo *combo)
{
	if (!combo)
		return;
	mutex_lock(&combo->transition);
	combo->removing = true;
	combo->setup_status = -ENODEV;
	complete_all(&combo->setup_done);
	mutex_unlock(&combo->transition);
}

static void btmtksdio_combo_release(void *data)
{
	struct mt7663_combo *combo = data;

	btmtksdio_combo_detach(combo);
	mutex_lock(&mt7663_combos_lock);
	list_del(&combo->node);
	mutex_unlock(&mt7663_combos_lock);
	mt7663_combo_put(combo);
}

static int btmtksdio_combo_register(struct btmtksdio_dev *bdev)
{
	struct mt7663_combo *combo;

	if (bdev->data->chipid != 0x7663 ||
	    !of_machine_is_compatible("zte,w103d"))
		return 0;
	combo = kzalloc(sizeof(*combo), GFP_KERNEL);
	if (!combo)
		return -ENOMEM;
	combo->bt_func = bdev->func;
	get_device(&bdev->func->dev);
	mutex_init(&combo->transition);
	init_completion(&combo->setup_done);
	refcount_set(&combo->refs, 1);
	combo->setup_status = -EINPROGRESS;
	bdev->combo = combo;
	mutex_lock(&mt7663_combos_lock);
	list_add_tail(&combo->node, &mt7663_combos);
	mutex_unlock(&mt7663_combos_lock);
	return devm_add_action_or_reset(bdev->dev, btmtksdio_combo_release,
					combo);
}
