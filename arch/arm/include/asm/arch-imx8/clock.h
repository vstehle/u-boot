/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2018 NXP
 */

#ifndef __ASM_ARCH_IMX8_CLOCK_H__
#define __ASM_ARCH_IMX8_CLOCK_H__

/* Mainly for compatible to imx common code. */
enum mxc_clock {
	MXC_ARM_CLK = 0,
	MXC_AHB_CLK,
	MXC_IPG_CLK,
	MXC_UART_CLK,
	MXC_CSPI_CLK,
	MXC_AXI_CLK,
	MXC_DDR_CLK,
	MXC_ESDHC_CLK,
	MXC_ESDHC2_CLK,
	MXC_ESDHC3_CLK,
	MXC_I2C_CLK,
	MXC_FEC_CLK,
};

u32 mxc_get_clock(enum mxc_clock clk);

#define PLL_USB_EN_USB_CLKS_MASK	(1 << 6)
#define PLL_USB_PWR_MASK		(1 << 12)
#define PLL_USB_ENABLE_MASK		(1 << 13)
#define PLL_USB_BYPASS_MASK		(1 << 16)
#define PLL_USB_REG_ENABLE_MASK		(1 << 21)
#define PLL_USB_DIV_SEL_MASK		(7 << 22)
#define PLL_USB_LOCK_MASK		(1 << 31)

#endif /* __ASM_ARCH_IMX8_CLOCK_H__ */
