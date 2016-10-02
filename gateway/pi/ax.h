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

#include <stdlib.h>
#include <stdint.h>

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

/* Which PLL */
enum ax_pll {
  AX_PLL_A,
  AX_PLL_B
};
/* Clock source type */
enum ax_clock_source_type {
  AX_CLOCK_SOURCE_CRYSTAL,
  AX_CLOCK_SOURCE_TCXO,
};
/* VCO type - See Datasheet Table 8. */
enum ax_vco_type {
  AX_VCO_INTERNAL = 0,
  AX_VCO_INTERNAL_EXTERNAL_INDUCTOR,
  AX_VCO_EXTERNAL,
};
/* Divider at the output of the VCO  */
enum ax_rfdiv {
  AX_RFDIV_UKNOWN = 0,
  AX_RFDIV_0,
  AX_RFDIV_1,
};


/**
 * represents the chosen modulation scheme
 */
typedef struct ax_modulation {
  uint8_t modulation;           /* modulation */
  uint8_t encoding;             /* encoding */
  uint8_t framing;              /* framing */
  uint32_t bitrate;             /* link bitrate provided to user */
  uint8_t fec;                  /* 0 = no fec, 1 = fec enabled */

  float power;                  /* TX output power */

  union {
    struct {                    /* FSK */
      float modulation_index;
    } fsk;
    struct {                    /* AFSK */
      uint16_t deviation;       /* (Hz) */
      uint16_t space, mark;     /* (Hz) */
    } afsk;
  } parameters;

  uint32_t max_delta_carrier;   /* max. delta from carrier centre, autoset if 0 */
  /* larger increases the time for the AFC to achieve lock */

} ax_modulation;


/**
 * represents one of the two physical synthesisers
 */
typedef struct ax_synthesiser {
  uint32_t frequency;
  uint32_t register_value;
  enum ax_rfdiv rfdiv;     /* set if this is known, else it's set automatically */
  uint8_t vco_range_known; /* set to 0 if vco range unknown */
  uint8_t vco_range;       /* determined by autoranging */
} ax_synthesiser;


/**
 *
 */
typedef struct ax_config {
  /* power mode */
  uint8_t pwrmode;

  /* synthesiser */
  struct {
    ax_synthesiser A, B;
    enum ax_vco_type vco_type;  /* default is internal */
  } synthesiser;

  /* external clock */
  enum ax_clock_source_type clock_source; /* Crystal or TCXO */
  uint32_t f_xtal;              /* external clock frequency (Hz) */
  uint16_t load_capacitance;    /* if crystal, load capacitance to be applied (pF) */
  uint32_t error_ppm;           /* max. error of clock source, ppm */
  uint8_t f_xtaldiv;            /* xtal division factor, set automatically */


  /* pll vco */
  uint32_t f_pllrng;

  /* rx parameters */
  uint8_t decimation;

} ax_config;


void ax_tx_on(ax_config* config, ax_modulation* mod);
void ax_tx_packet(ax_config* config, uint8_t* packet, uint16_t length);

void ax_init(ax_config* config);

uint8_t ax_silicon_revision(int channel);
uint8_t ax_scratch(int channel);

#endif  /* AX_H */
