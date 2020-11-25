/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020 ARM Limited
 * (C) Copyright 2020 Linaro
 * Rui Miguel Silva <rui.silva@linaro.org>
 *
 * Configuration for Corstone700. Parts were derived from other ARM
 * configurations.
 */

#ifndef __CORSTONE700_H
#define __CORSTONE700_H

#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x03f00000)
#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_SYS_HZ		1000

#define V2M_SRAM0		0x02000000
#define V2M_QSPI		0x08000000

#define V2M_DEBUG		0x10000000
#define V2M_BASE_PERIPH		0x1A000000

#define V2M_BASE		0x80000000

#define V2M_PERIPH_OFFSET(x)	(x << 16)

#define V2M_SYSID		(V2M_BASE_PERIPH)
#define V2M_SYSCTL		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(1))

#define V2M_COUNTER_CTL		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(32))
#define V2M_COUNTER_READ	(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(33))

#define V2M_TIMER_CTL		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(34))
#define V2M_TIMER_BASE0		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(35))

#define V2M_UART0		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(81))
#define V2M_UART1		(V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(82))

#define CONFIG_PL011_CLOCK	32000000

/* Physical Memory Map */
#define PHYS_SDRAM_1		(V2M_BASE)
#define PHYS_SDRAM_2		(V2M_QSPI)

/* Top 16MB reserved for secure world use (maybe not needed) */
#define DRAM_SEC_SIZE		0x01000000
#define PHYS_SDRAM_1_SIZE	0x80000000 - DRAM_SEC_SIZE

#define PHYS_SDRAM_2_SIZE	0x02000000

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + (8 << 20))

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR	(V2M_BASE + 0x10000000)

#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#define CONFIG_EXTRA_ENV_SETTINGS	\
				"kernel_name=zImage\0"		\
				"kernel_addr_r=0x08300000\0"	\
				"fdt_name=devtree.dtb\0"	\
				"fdt_addr_r=0x80400000\0"	\
				"fdt_high=0xffffffff\0"

#define CONFIG_BOOTCOMMAND	"bootx $kernel_addr_r $fdt_addr_r"

//#define CONFIG_SYS_MAX_FLASH_BANKS	64
//#define CONFIG_SYS_FLASH_CFI
//#define CONFIG_SYS_FLASH_BASE		0x08000000

#endif
