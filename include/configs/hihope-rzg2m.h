/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * include/configs/hihope-rzg2m.h
 *     This file is HOPERUN HiHope-RZG2M board configuration.
 *
 * Copyright (C) 2019 Renesas Electronics Corporation
 */

#ifndef __HIHOPE_RZG2M_H
#define __HIHOPE_RZG2M_H

#include "rcar-gen3-common.h"

/* Ethernet RAVB */
#define CONFIG_BITBANGMII
#define CONFIG_BITBANGMII_MULTI

/* Generic Timer Definitions (use in assembler source) */
#define COUNTER_FREQUENCY	0xFE502A	/* 16.66MHz from CPclk */

#define CONFIG_BOARD_LATE_INIT

#endif /* __HIHOPE_RZG2M_H */
