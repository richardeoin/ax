/*
 * Functions for controlling ax radios
 * Copyright (C) 2016  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AX_H
#define AX_H

/**
 * ax status field
 */
#define AX_STATUS_POWER_READY				(1 << 15)
#define AX_STATUS_PLL_LOCK					(1 << 14)
#define AX_STATUS_FIFO_OVERFLOW				(1 << 13)
#define AX_STATUS_FIFO_UNDERFLOW			(1 << 12)
#define AX_STATUS_THRESHOLD_FREE			(1 << 11)
#define AX_STATUS_THRESHOLD_COUNT			(1 << 10)
#define AX_STATUS_FIFO_FULL					(1 << 9)
#define AX_STATUS_FIFO_EMPTY				(1 << 8)
#define AX_STATUS_PWRGOOD					(1 << 7)
#define AX_STATUS_PWR_INTERRUPT_PENDING		(1 << 6)
#define AX_STATUS_RADIO_EVENT_PENDING		(1 << 5)
#define AX_STATUS_XTAL_OSCILLATOR_RUNNING	(1 << 4)
#define AX_STATUS_WAKEUP_INTERRUPT_PENDING	(1 << 3)
#define AX_STATUS_LPOSC_INTERRUPT_PENDING	(1 << 2)
#define AX_STATUS_GPADC_INTERRUPT_PENDING	(1 << 1)

/**
 * ax constants
 */
#define AX_SILICONREVISION	0x51
#define AX_SCRATCH			0xC5



uint8_t ax_silicon_revision(int channel);
uint8_t ax_scratch(int channel);

#endif  /* AX_H */
