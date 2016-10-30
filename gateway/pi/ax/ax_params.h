/*
 * Calculations for ax parameters
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

#ifndef AX_PARAMS_H
#define AX_PARAMS_H

#include <stdint.h>
#include <math.h>

#include "ax.h"

/**
 * 5.15.15 AGCGAIN
 */
static uint8_t ax_rx_agcgain(ax_config* config, uint32_t f_3dB)
{
  const float PI = 3.1415927;

  float ratio = (64.0 * PI * config->f_xtaldiv * f_3dB) /
    (float)config->f_xtal;

  return (uint8_t)(-log2(1 - sqrt(1 - ratio)));
}

/**
 * 5.15.24 FREQGAINC/D
 */
static uint8_t ax_rx_freqgain_rf_recovery_gain(ax_config* config, uint32_t freq)
{
  float ratio = (float)config->f_xtal / (config->f_xtaldiv * 4 * freq);

  return (uint8_t)(log2(ratio) + 0.5);
}

/**
 * Converts a value to 4-bit mantissa and 4-bit exponent
 */
static uint8_t ax_value_to_mantissa_exp_4_4(uint32_t value)
{
  uint8_t exp = 0;

  while (value > 15 && exp < 15) {
    value >>= 1; exp++;
  }

  return ((value & 0xF) << 4) | exp; /* mantissa, exponent */
}
/**
 * Converts a value to 3-bit exponent and 5-bit mantissa
 */
static uint8_t ax_value_to_exp_mantissa_3_5(uint32_t value)
{
  uint8_t exp = 0;

  while (value > 31 && exp < 7) {
    value >>= 1; exp++;
  }

  return ((exp & 0x7) << 5) | value; /* exponent, mantissa */
}


/* populates ax_params structure */
void ax_populate_params(ax_config* config, ax_modulation* mod, ax_params* par);

#endif  /* AX_PARAMS_H */
