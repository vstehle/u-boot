// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020, Vincent Stehlé <vincent.stehle@arm.com>
 * Based on virtio.c
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <dm/device-internal.h>

static int shb_curr_dev;

static void shb_init(void)
{
	struct udevice *it;
	struct uclass *uc;

	uclass_get(UCLASS_BLK, &uc);
	uclass_foreach_dev(it, uc) {
		if (!strcmp(it->name, "semihosting_blk"))
			device_probe(it);
	}
}

static int do_shb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc == 2 && !strcmp(argv[1], "scan")) {
		/* make sure all shb devices are enumerated */
		shb_init();

		return CMD_RET_SUCCESS;
	}

	return blk_common_cmd(argc, argv, IF_TYPE_SHB, &shb_curr_dev);
}

U_BOOT_CMD(
	shb, 8, 1, do_shb,
	"semihosting block devices",
	"scan - initialize shb\n"
	"shb info - show all available shb block devices\n"
	"shb device [dev] - show or set current shb block device\n"
	"shb part [dev] - print partition table of one or all shb block devices\n"
	"shb read addr blk# cnt - read `cnt' blocks starting at block\n"
	"     `blk#' to memory address `addr'\n"
	"shb write addr blk# cnt - write `cnt' blocks starting at block\n"
	"     `blk#' from memory address `addr'"
);
