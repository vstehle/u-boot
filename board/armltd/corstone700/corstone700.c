// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 ARM Limited
 * (C) Copyright 2020 Linaro
 * Rui Miguel Silva <rui.silva@linaro.org>
 */

#include <common.h>
#include <dm.h>
#include <dm/platform_data/serial_pl01x.h>
#include <malloc.h>

DECLARE_GLOBAL_DATA_PTR;

static const struct pl01x_serial_platdata serial_platdata = {
	.base = V2M_UART0,
	.type = TYPE_PL011,
	.clock = CONFIG_PL011_CLOCK,
};

U_BOOT_DEVICE(corstone700_serials) = {
	.name = "serial_pl01x",
	.platdata = &serial_platdata,
};

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

void reset_cpu(ulong addr)
{
}
