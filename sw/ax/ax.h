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
 * Initialisation Status
 */
typedef enum ax_init_status {
  AX_INIT_OK = 0,
  AX_INIT_PORT_FAILED,
  AX_INIT_BAD_SCRATCH,
  AX_INIT_BAD_REVISION,
  AX_INIT_SET_SPI,
  AX_INIT_VCO_RANGING_FAILED,
} ax_init_status;

/**
 * pin functions
 */
typedef uint8_t pinfunc_t;

/**
 * Represents a receive parameter set for an ax5243 radio
 */
typedef struct ax_rx_param_set {
  uint8_t agc_attack, agc_decay;
  uint32_t time_gain, dr_gain;
  uint8_t phase_gain, filter_idx;
  uint8_t baseband_rg_phase_det;
  uint8_t baseband_rg_freq_det;
  uint8_t rffreq_rg_phase_det;
  uint8_t rffreq_rg_freq_det;
  uint8_t amplgain, amplflags;
  uint16_t freq_dev;
} ax_rx_param_set;
/**
 * Represents the tweakable parameters for an ax5243 radio
 */
typedef struct ax_params {
  uint8_t is_params_set;           /* has this structure been set? */
  float m; // modulation index

  // 5.6 forward error correction
  uint8_t fec_inp_shift;
  uint8_t shortmem;

  // 5.15 receiver parameters
  uint32_t rx_bandwidth;
  uint32_t f_baseband;
  uint32_t if_frequency;
  uint32_t iffreq;
  uint32_t decimation;
  uint32_t rx_data_rate;
  uint32_t max_rf_offset;
  uint32_t fskd;
  uint8_t  ampl_filter;

  // 5.15.10 afskctrl
  uint8_t afskshift;

  // 5.15.15+ receiver parameter sets
  ax_rx_param_set rx_param_sets[4];

  // 5.20 packet format
  uint8_t fec_sync_dis;

  // 5.21 match parameters
  uint8_t match1_threashold;
  uint8_t match0_threashold;

  // 5.22 packet controller
  uint8_t pkt_misc_flags;
  uint16_t tx_pll_boost_time, tx_pll_settle_time;
  uint16_t rx_pll_boost_time, rx_pll_settle_time;
  uint16_t rx_coarse_agc;
  uint16_t rx_agc_settling, rx_rssi_settling;
  uint16_t preamble_1_timeout, preamble_2_timeout;
  uint8_t rssi_abs_thr;

  // 5.26 'performance tuning'
  uint8_t perftuning_option;

} ax_params;

/**
 * Represents the chosen modulation scheme.
 */
typedef struct ax_modulation {
  uint8_t modulation;           /* modulation */
  uint8_t encoding;             /* encoding */
  uint8_t framing;              /* framing */
  uint8_t shaping;              /* shaping */
  uint32_t bitrate;             /* symbol bitrate provided to user */
  uint8_t fec;                  /* 0 = no fec, 1 = fec enabled */

  float power;                  /* TX output power, as fraction of maximum */
  /* Pre-distortion is possible in hardware, but not supported here. */

  uint8_t continuous;           /* 0 = occasional packets, 1 = continuous tx */

  uint8_t fixed_packet_length;  /* 0 = variable length, 1-255 = length */

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

  ax_params par;                /* tweakable parameters */

} ax_modulation;

/**
 * CONFIG ----------------------------------------------------------------------
 */

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
/* Transmit path */
enum ax_transmit_path {
  AX_TRANSMIT_PATH_DIFF = 0,
  AX_TRANSMIT_PATH_SE,
};


/**
 * Represents one of the two physical synthesisers.
 */
typedef struct ax_synthesiser {
  uint32_t frequency;
  uint32_t register_value;
  enum ax_rfdiv rfdiv;     /* set if this is known, else it's set automatically */
  uint32_t frequency_when_last_ranged;
  uint8_t vco_range_known; /* set to 0 if vco range unknown */
  uint8_t vco_range;       /* determined by autoranging */
} ax_synthesiser;

/**
 * Represents a received packet and its metadata
 */
typedef struct ax_packet {
  unsigned char data[0x200];
  uint16_t length;
  int16_t rssi;
  int32_t rffreqoffs;
} ax_packet;

/**
 * configuration
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
  void* (*tcxo_enable)(void);    /* function to enable tcxo */
  void* (*tcxo_disable)(void);   /* function to disable tcxo */
  enum ax_transmit_path transmit_path; /* transmit path */

  /* spi transfer */
  void (*spi_transfer)(unsigned char*, uint8_t);

  /* receive */
  uint8_t pkt_store_flags;      /* PKTSTOREFLAGS */

  /* wakeup */
  uint32_t wakeup_period_ms;
  uint32_t wakeup_xo_early_ms;

  /* pll vco */
  uint32_t f_pllrng;

} ax_config;

/**
 * configuration for wakeup
 */
typedef struct ax_wakeup_config {
  uint32_t wakeup_period_ms;    /* period of wakeups, in ms */
  uint32_t wakeup_xo_early_ms;  /* wakeup the XO before, in ms */

  uint32_t wakeup_duration_bits; /* length the wakeup event if no packet is rx'd */
  /* suggest 25 */

  uint8_t rssi_abs_thr;         /* rssi threashold for wakeup event to happen  */
  /* suggest 221, or 3log2(b/w) + x  */
} ax_wakeup_config;

/**
 * FUNCTION PROTOTYPES ---------------------------------------------------------
 */

/* tweakable parameters */
void ax_default_params(ax_config* config, ax_modulation* mod);

/* adjust frequency */
int ax_adjust_frequency(ax_config* config, uint32_t frequency);
int ax_force_quick_adjust_frequency(ax_config* config, uint32_t frequency);

/* transmit */
void ax_tx_on(ax_config* config, ax_modulation* mod);
void ax_tx_packet(ax_config* config, ax_modulation* mod,
                  uint8_t* packet, uint16_t length);
void ax_tx_1k_zeros(ax_config* config);

/* receive */
void ax_rx_on(ax_config* config, ax_modulation* mod);
void ax_rx_wor(ax_config* config, ax_modulation* mod,
               ax_wakeup_config* wakeup_config);
int ax_rx_packet(ax_config* config, ax_packet* rx_pkt);

/* turn off */
void ax_off(ax_config* config);
void ax_force_off(ax_config* config);

/* pinfunc */
void ax_set_pinfunc_sysclk(ax_config* config, pinfunc_t func);
void ax_set_pinfunc_dclk(ax_config* config, pinfunc_t func);
void ax_set_pinfunc_data(ax_config* config, pinfunc_t func);
void ax_set_pinfunc_antsel(ax_config* config, pinfunc_t func);
void ax_set_pinfunc_pwramp(ax_config* config, pinfunc_t func);

/* tx path */
void ax_set_tx_path(ax_config* config, enum ax_transmit_path path);

/* init */
int ax_init(ax_config* config);

#endif  /* AX_H */
