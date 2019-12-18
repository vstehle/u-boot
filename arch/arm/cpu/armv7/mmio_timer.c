// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 */

#include <common.h>
#include <asm/io.h>
#include <div64.h>
#include <bootstage.h>

DECLARE_GLOBAL_DATA_PTR;

#define CNTCTLBASE    0x1a020000UL
#define CNTREADBASE   0x1a030000UL

static inline uint32_t mmio_read32(uintptr_t addr)
{
      return *(volatile uint32_t*)addr;
}

int timer_init(void)
{
      gd->arch.timer_rate_hz = mmio_read32(CNTCTLBASE);

      return 0;
}

unsigned long long get_ticks(void)
{
      return ((mmio_read32(CNTCTLBASE + 0x4) << 32) |
              mmio_read32(CNTREADBASE));
}

ulong get_timer(ulong base)
{
      return lldiv(get_ticks(), gd->arch.timer_rate_hz) - base;
}

void __udelay(unsigned long usec)
{
      unsigned long endtime;

      endtime = lldiv((unsigned long long)usec * gd->arch.timer_rate_hz,
                      1000UL);

      endtime += get_ticks();

      while (get_ticks() < endtime)
              ;
}

ulong get_tbclk(void)
{
      return gd->arch.timer_rate_hz;
}
