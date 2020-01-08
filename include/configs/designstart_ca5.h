/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020 ARM Limited
 * (C) Copyright 2020 Linaro
 * Rui Miguel Silva <rui.silva@linaro.org>
 *
 * Configuration for Cortex-A5 Designstart. Parts were derived from other ARM
 * configurations.
 */

#ifndef __DESISGNSTART_CA5_H
#define __DESISGNSTART_CA5_H

#define CONFIG_SYS_INIT_SP_ADDR               (CONFIG_SYS_SDRAM_BASE + 0x03f00000)
#define CONFIG_SKIP_LOWLEVEL_INIT

/* Generic Timer Definitions */
#define CONFIG_SYS_HZ_CLOCK   7500000
#define CONFIG_SYS_HZ         1000
#define COUNTER_FREQUENCY     CONFIG_SYS_HZ_CLOCK

#ifdef CONFIG_DESIGNSTART_MEMORY_MAP_EXTENDED
#define V2M_SRAM0             0x00010000
#define V2M_SRAM1             0x02200000
#define V2M_QSPI              0x0A800000
#else
#define V2M_SRAM0             0x00000000
#define V2M_SRAM1             0x02000000
#define V2M_QSPI              0x08000000
#endif

#define V2M_DEBUG             0x10000000
#define V2M_BASE_PERIPH               0x1A000000
#define V2M_A5_PERIPH         0x1C000000
#define V2M_L2CC_PERIPH               0x1C010000

#define V2M_MASTER_EXPANSION0 0x40000000
#define V2M_MASTER_EXPANSION1 0x60000000

#define V2M_BASE              0x80000000

#define V2M_PERIPH_OFFSET(x)  (x << 16)

#define V2M_SYSID             (V2M_BASE_PERIPH)
#define V2M_SYCTL             (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(1))
#define V2M_COUNTER_CTL               (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(2))
#define V2M_COUNTER_READ      (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(3))
#define V2M_TIMER_CTL         (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(4))
#define V2M_TIMER0            (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(5))

#define V2M_WATCHDOG_CTL      (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(16))
#define V2M_WATCHDOG_REFRESH  (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(17))

#define V2M_UART0             (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(32))
#define V2M_UART1             (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(33))

#define V2M_RTC                       (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(34))
#define V2M_TRNG              (V2M_BASE_PERIPH + V2M_PERIPH_OFFSET(35))

/* PL011 Serial Configuration */
#define CONFIG_CONS_INDEX     0
#define CONFIG_PL011_CLOCK    7500000

/* Physical Memory Map */
#define PHYS_SDRAM_1          (V2M_BASE)

/* Top 16MB reserved for secure world use */
#define DRAM_SEC_SIZE         0x01000000
#define PHYS_SDRAM_1_SIZE     0x80000000 - DRAM_SEC_SIZE

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + (8 << 20))

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR  (V2M_BASE + 0x10000000)

#define CONFIG_SYS_SDRAM_BASE PHYS_SDRAM_1

#define CONFIG_SYS_MMIO_TIMER

/* Enable memtest */
#define CONFIG_SYS_MEMTEST_START      PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END                (PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)

#define CONFIG_EXTRA_ENV_SETTINGS     \
                              "kernel_name=Image\0"           \
                              "kernel_addr=0x80F00000\0"      \
                              "initrd_name=ramdisk.img\0"     \
                              "initrd_addr=0x84000000\0"      \
                              "fdt_name=devtree.dtb\0"        \
                              "fdt_addr=0x83000000\0"         \
                              "fdt_high=0xffffffff\0"         \
                              "initrd_high=0xffffffff\0"

#define CONFIG_BOOTCOMMAND    "echo copy to RAM...; " \
                              "cp.b 0x80100000 $kernel_addr 0xB00000; " \
                              "cp.b 0x80D00000 $initrd_addr 0x800000; " \
                              "bootz $kernel_addr $initrd_addr $fdt_addr"

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE             512     /* Console I/O Buffer Size */
#define CONFIG_SYS_MAXARGS            64      /* max command args */

#define CONFIG_SYS_FLASH_BASE         0x80000000
/* 256 x 256KiB sectors */
#define CONFIG_SYS_MAX_FLASH_SECT     256
/* Store environment at top of flash */
#define CONFIG_ENV_ADDR                       0x0A7C0000
#define CONFIG_ENV_SECT_SIZE          0x00040000

#define CONFIG_SYS_FLASH_CFI          1
#define CONFIG_FLASH_CFI_DRIVER               1
#define CONFIG_SYS_FLASH_CFI_WIDTH    FLASH_CFI_32BIT
#define CONFIG_SYS_MAX_FLASH_BANKS    1

#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE /* use buffered writes */
#define CONFIG_SYS_FLASH_PROTECTION   /* The devices have real protection */
#define CONFIG_SYS_FLASH_EMPTY_INFO   /* flinfo indicates empty blocks */
#define FLASH_MAX_SECTOR_SIZE         0x00040000
#define CONFIG_ENV_SIZE                       CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_IS_IN_FLASH                1
#endif
