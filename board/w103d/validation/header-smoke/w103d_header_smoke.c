// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>

static int __init w103d_header_smoke_init(void)
{
	return 0;
}

static void __exit w103d_header_smoke_exit(void)
{
}

module_init(w103d_header_smoke_init);
module_exit(w103d_header_smoke_exit);

MODULE_DESCRIPTION("W103D packaged-kernel-header smoke test");
MODULE_LICENSE("GPL");
