/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2012
 * Vincent Stehlé, Texas Instruments, v-stehle@ti.com.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

/* OMAP5 specific function to set up the identity mapping. */
void arm_setup_identity_mapping(u32 *page_table)
{
	/* Perform default mapping, which sets up an identity-mapping for all
	 * 4GB, rw for everyone */
	__arm_setup_identity_mapping();

	/* First page (starting at 0x0) is made invalid to avoid speculative
	 * accesses in secure rom. TODO: use second level descriptors for finer
	 * grained mapping. */
	page_table[0] = 0;
}
