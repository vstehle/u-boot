// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 NXP
 * Copyright 2021 Linaro
 */

#include <common.h>
#include <dwc3-uboot.h>
#include <errno.h>
#include <miiphy.h>
#include <netdev.h>
#include <spl.h>
#include <usb.h>
#include <asm/io.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/arch/clock.h>
#include <asm/mach-imx/dma.h>
#include <linux/delay.h>
#include <power/pmic.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_FSEL1)
#define WDOG_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_ODE | PAD_CTL_PUE | PAD_CTL_PE)

static iomux_v3_cfg_t const uart_pads[] = {
	MX8MP_PAD_ECSPI1_SCLK__UART3_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX8MP_PAD_ECSPI1_MOSI__UART3_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const wdog_pads[] = {
	MX8MP_PAD_GPIO1_IO02__WDOG1_WDOG_B  | MUX_PAD_CTRL(WDOG_PAD_CTRL),
};

#ifdef CONFIG_NAND_MXS
static void setup_gpmi_nand(void)
{
	init_nand_clk();
}
#endif

int board_early_init_f(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;

	imx_iomux_v3_setup_multiple_pads(wdog_pads, ARRAY_SIZE(wdog_pads));

	set_wdog_reset(wdog);

	imx_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));

	init_uart_clk(2);

	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, struct bd_info *bd)
{
	return 0;
}
#endif

#ifdef CONFIG_FEC_MXC
#define FEC_RST_PAD IMX_GPIO_NR(4, 2)
static iomux_v3_cfg_t const fec1_rst_pads[] = {
	MX8MP_PAD_SAI1_RXD0__GPIO4_IO02 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec1_rst_pads,
					 ARRAY_SIZE(fec1_rst_pads));

	gpio_request(FEC_RST_PAD, "fec1_rst");
	gpio_direction_output(FEC_RST_PAD, 0);
	mdelay(15);
	gpio_direction_output(FEC_RST_PAD, 1);
	mdelay(100);
}

static int setup_fec(void)
{
	struct iomuxc_gpr_base_regs *gpr =
		(struct iomuxc_gpr_base_regs *)IOMUXC_GPR_BASE_ADDR;

	setup_iomux_fec();

	/* Enable RGMII TX clk output */
	setbits_le32(&gpr->gpr[1], BIT(22));

	//return set_clk_enet(ENET_125MHZ);
	return 0;
}
#endif /* CONFIG_FEC_MXC */

#ifdef CONFIG_DWC_ETH_QOS
#define EQOS_RST_PAD IMX_GPIO_NR(4, 22)
static iomux_v3_cfg_t const eqos_rst_pads[] = {
	MX8MP_PAD_SAI2_RXC__GPIO4_IO22 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_eqos(void)
{
	imx_iomux_v3_setup_multiple_pads(eqos_rst_pads,
					 ARRAY_SIZE(eqos_rst_pads));

	gpio_request(EQOS_RST_PAD, "eqos_rst");
	gpio_direction_output(EQOS_RST_PAD, 0);
	mdelay(15);
	gpio_direction_output(EQOS_RST_PAD, 1);
	mdelay(100);
}

static int setup_eqos(void)
{
	struct iomuxc_gpr_base_regs *gpr =
		(struct iomuxc_gpr_base_regs *)IOMUXC_GPR_BASE_ADDR;

	setup_iomux_eqos();

	/* set INTF as RGMII, enable RGMII TXC clock */
	clrsetbits_le32(&gpr->gpr[1],
			IOMUXC_GPR_GPR1_GPR_ENET_QOS_INTF_SEL_MASK, BIT(16));
	setbits_le32(&gpr->gpr[1], BIT(19) | BIT(21));

	return set_clk_eqos(ENET_125MHZ);
}
#endif /* CONFIG_DWC_ETH_QOS */

int board_phy_config(struct phy_device *phydev)
{
	if (IS_ENABLED(CONFIG_FEC_MXC) || IS_ENABLED(CONFIG_DWC_ETH_QOS)) {
		/* enable rgmii rxc skew and phy mode select to RGMII copper */
		phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x1f);
		phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x8);

		phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x00);
		phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x82ee);
		phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x05);
		phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x100);

		if (phydev->drv->config)
			phydev->drv->config(phydev);
	}

	return 0;
}

#ifdef CONFIG_USB_DWC3
#define USB_PHY_CTRL0			0xF0040
#define USB_PHY_CTRL0_REF_SSP_EN	BIT(2)

#define USB_PHY_CTRL1			0xF0044
#define USB_PHY_CTRL1_RESET		BIT(0)
#define USB_PHY_CTRL1_COMMONONN		BIT(1)
#define USB_PHY_CTRL1_ATERESET		BIT(3)
#define USB_PHY_CTRL1_VDATSRCENB0	BIT(19)
#define USB_PHY_CTRL1_VDATDETENB0	BIT(20)

#define USB_PHY_CTRL2			0xF0048
#define USB_PHY_CTRL2_TXENABLEN0	BIT(8)

#define USB_PHY_CTRL6			0xF0058

#define HSIO_GPR_BASE                               (0x32F10000U)
#define HSIO_GPR_REG_0                              (HSIO_GPR_BASE)
#define HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN_SHIFT    (1)
#define HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN          (0x1U << \
				HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN_SHIFT)

#define USB1_BASE_ADDR          0x38100000

static struct dwc3_device dwc3_device_data = {
#ifdef CONFIG_SPL_BUILD
	.maximum_speed = USB_SPEED_HIGH,
#else
	.maximum_speed = USB_SPEED_SUPER,
#endif
	.base = USB1_BASE_ADDR,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
};

int usb_gadget_handle_interrupts(void)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

static void dwc3_nxp_usb_phy_init(struct dwc3_device *dwc3)
{
	u32 RegData;

	/* enable usb clock via hsio gpr */
	RegData = readl(HSIO_GPR_REG_0);
	RegData |= HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN;
	writel(RegData, HSIO_GPR_REG_0);

	/* USB3.0 PHY signal fsel for 100M ref */
	RegData = readl(dwc3->base + USB_PHY_CTRL0);
	RegData = (RegData & 0xfffff81f) | (0x2a << 5);
	writel(RegData, dwc3->base + USB_PHY_CTRL0);

	RegData = readl(dwc3->base + USB_PHY_CTRL6);
	RegData &= ~0x1;
	writel(RegData, dwc3->base + USB_PHY_CTRL6);

	RegData = readl(dwc3->base + USB_PHY_CTRL1);
	RegData &= ~(USB_PHY_CTRL1_VDATSRCENB0 | USB_PHY_CTRL1_VDATDETENB0 |
			USB_PHY_CTRL1_COMMONONN);
	RegData |= USB_PHY_CTRL1_RESET | USB_PHY_CTRL1_ATERESET;
	writel(RegData, dwc3->base + USB_PHY_CTRL1);

	RegData = readl(dwc3->base + USB_PHY_CTRL0);
	RegData |= USB_PHY_CTRL0_REF_SSP_EN;
	writel(RegData, dwc3->base + USB_PHY_CTRL0);

	RegData = readl(dwc3->base + USB_PHY_CTRL2);
	RegData |= USB_PHY_CTRL2_TXENABLEN0;
	writel(RegData, dwc3->base + USB_PHY_CTRL2);

	RegData = readl(dwc3->base + USB_PHY_CTRL1);
	RegData &= ~(USB_PHY_CTRL1_RESET | USB_PHY_CTRL1_ATERESET);
	writel(RegData, dwc3->base + USB_PHY_CTRL1);
}
#endif /* CONFIG_USB_DWC3 */

#if defined(CONFIG_USB_DWC3) || defined(CONFIG_USB_XHCI_IMX8M)
#define FSL_SIP_GPC			0xC2000000
#define FSL_SIP_CONFIG_GPC_PM_DOMAIN	0x03

#ifdef CONFIG_SPL_BUILD
static u32 gpc_pu_m_core_offset[11] = {
	0xc00, 0xc40, 0xc80, 0xcc0,
	0xdc0, 0xe00, 0xe40, 0xe80,
	0xec0, 0xf00, 0xf40,
};

#define PGC_PCR				0

void imx_gpc_set_m_core_pgc(unsigned int offset, bool pdn)
{
	u32 val;
	uintptr_t reg = GPC_BASE_ADDR + offset;

	val = readl(reg);
	val &= ~(0x1 << PGC_PCR);

	if (pdn)
		val |= 0x1 << PGC_PCR;
	writel(val, reg);
}

void imx8m_usb_power_domain(u32 domain_id, bool on)
{
	u32 val;
	uintptr_t reg;

	imx_gpc_set_m_core_pgc(gpc_pu_m_core_offset[domain_id], true);

	reg = GPC_BASE_ADDR + (on ? 0xf8 : 0x104);
	val = 1 << (domain_id > 3 ? (domain_id + 3) : domain_id);
	writel(val, reg);
	while (readl(reg) & val)
		;
	imx_gpc_set_m_core_pgc(gpc_pu_m_core_offset[domain_id], false);
}
#endif

int imx8m_usb_power(int usb_id, bool on)
{
	if (usb_id > 1)
		return -EINVAL;

#ifdef CONFIG_SPL_BUILD
	imx8m_usb_power_domain(2 + usb_id, on);
#else
	unsigned long ret = 0;
	//	ret = call_imx_sip(FSL_SIP_GPC,
	//		FSL_SIP_CONFIG_GPC_PM_DOMAIN, 2 + usb_id, on, 0);
	if (ret)
		return -EPERM;
#endif

	return 0;
}

#define USB2_PWR_EN IMX_GPIO_NR(1, 14)
int board_usb_init(int index, enum usb_init_type init)
{
	int ret = 0;

	imx8m_usb_power(index, true);

	if (index == 0 && init == USB_INIT_DEVICE) {
		dwc3_nxp_usb_phy_init(&dwc3_device_data);
		return dwc3_uboot_init(&dwc3_device_data);
	} else if (index == 0 && init == USB_INIT_HOST) {
		return ret;
	} else if (index == 1 && init == USB_INIT_HOST) {
		/* Enable GPIO1_IO14 for 5V VBUS */
		gpio_request(USB2_PWR_EN, "usb2_pwr");
		gpio_direction_output(USB2_PWR_EN, 1);
	}

	return 0;
}

int board_usb_cleanup(int index, enum usb_init_type init)
{
	int ret = 0;

	if (index == 0 && init == USB_INIT_DEVICE) {
		dwc3_uboot_exit(index);
	} else if (index == 1 && init == USB_INIT_HOST) {
		/* Disable GPIO1_IO14 for 5V VBUS */
		gpio_direction_output(USB2_PWR_EN, 0);
	}

	imx8m_usb_power(index, false);

	return ret;
}
#endif

#define DISPMIX				13
#define MIPI				15

#define WDOG_TRIG IMX_GPIO_NR(4, 20)

static iomux_v3_cfg_t wdt_trig[] = {
	MX8MP_PAD_SAI1_MCLK__GPIO4_IO20 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_wdt(void)
{
	imx_iomux_v3_setup_multiple_pads(wdt_trig, ARRAY_SIZE(wdt_trig));
	gpio_request(WDOG_TRIG, "wdt_trig");
	gpio_direction_output(WDOG_TRIG, 1);
}

int board_init(void)
{
#ifdef CONFIG_FEC_MXC
	setup_fec();
#endif

#ifdef CONFIG_DWC_ETH_QOS
	/* clock, pin, gpr */
	setup_eqos();
#endif

#ifdef CONFIG_NAND_MXS
	setup_gpmi_nand();
#endif

	setup_iomux_wdt();

	return 0;
}

int board_late_init(void)
{
	if (IS_ENABLED(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)) {
		env_set("board_name", "RSB3720A1");
		env_set("board_rev", "iMX8MP");
	}

	return 0;
}

#ifdef CONFIG_SPL_MMC_SUPPORT
#define UBOOT_RAW_SECTOR_OFFSET 0x40
unsigned long spl_mmc_get_uboot_raw_sector(struct mmc *mmc)
{
	u32 boot_dev = spl_boot_device();

	switch (boot_dev) {
	case BOOT_DEVICE_MMC2:
		return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR - UBOOT_RAW_SECTOR_OFFSET;
	default:
		return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR;
	}
}
#endif /* CONFIG_SPL_MMC_SUPPORT */
