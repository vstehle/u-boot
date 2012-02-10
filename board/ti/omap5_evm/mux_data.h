/*
 * (C) Copyright 2010
 * Texas Instruments Incorporated, <www.ti.com>
 *
 *	Sricharan R		<r.sricharan@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _EVM5430_MUX_DATA_H
#define _EVM5430_MUX_DATA_H

#include <asm/arch/mux_omap5.h>

const struct pad_conf_entry core_padconf_array_essential[] = {

	{SDCARD_CLK, (PTU | IEN | M0)}, /*  SDCARD_CLK  */
	{SDCARD_CMD, (PTU | IEN | M0)}, /*  SDCARD_CMD  */
	{SDCARD_DATA0, (PTU | IEN | M0)}, /*  SDCARD_DATA0*/
	{SDCARD_DATA1, (PTU | IEN | M0)}, /*  SDCARD_DATA1*/
	{SDCARD_DATA2, (PTU | IEN | M0)}, /*  SDCARD_DATA2*/
	{SDCARD_DATA3, (PTU | IEN | M0)}, /*  SDCARD_DATA3*/
	{UART3_RX_IRRX, (PTU | IEN | M0)}, /*  UART3_RX_IRRX    */
	{UART3_TX_IRTX, (M0)},    /*  UART3_TX_IRTX    */

};

const struct pad_conf_entry wkup_padconf_array_essential[] = {

	{SR_PMIC_SCL, (PTU | IEN | M0)}, /* SR_PMIC_SCL */
	{SR_PMIC_SDA, (PTU | IEN | M0)}, /* SR_PMIC_SDA */
	{SYS_32K, (IEN | M0)}, /*  SYS_32K     */

};

const struct pad_conf_entry core_padconf_array_non_essential[] = {

};

const struct pad_conf_entry wkup_padconf_array_non_essential[] = {


};

#endif /* _EVM4430_MUX_DATA_H */
