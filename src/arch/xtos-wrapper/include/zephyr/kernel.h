/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2022 Intel Corporation. All rights reserved.
 *
 * Author: Jyri Sarha <jyri.sarha@intel.com>
 */

#ifndef __XTOS_WRAPPER_KERNEL_H__
#define __XTOS_WRAPPER_KERNEL_H__

#include <sof/lib/wait.h>

#include <stdint.h>

#ifdef __ZEPHYR__
#error "This file should only be included in XTOS builds."
#endif

typedef uint32_t k_ticks_t;

typedef struct {
	k_ticks_t ticks;
} k_timeout_t;

#define Z_TIMEOUT_TICKS(t) ((k_timeout_t) { .ticks = (t) })

#define Z_TIMEOUT_US(t) ((k_timeout_t) { .ticks = clock_us_to_ticks(PLATFORM_DEFAULT_CLOCK, t) })

static inline void k_sleep(k_timeout_t timeout)
{
	wait_delay(timeout.ticks);
}

static inline void k_msleep(int32_t ms)
{
	wait_delay_ms(ms);
}

static inline void k_usleep(int32_t us)
{
	wait_delay_us(us);
}

#endif /* __XTOS_WRAPPER_KERNEL_H__ */
