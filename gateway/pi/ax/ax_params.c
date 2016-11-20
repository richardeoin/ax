/*
 * Calcuates tweakable parameters for an ax radio
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define USE_MATH_H
#ifdef USE_MATH_H
#include <math.h>
#endif

#include "ax.h"
#include "ax_reg_values.h"
#include "ax_params.h"

#include <stdio.h>
#define debug_printf printf

/**
 * 5.6 forward error correction
 */
void ax_param_forward_error_correction(ax_config* config, ax_modulation* mod,
                                       ax_params* par)
{
  (void)config;
  (void)mod;

  par->fec_inp_shift = 1;
  par->shortmem = 0;
}

/**
 * 5.15 receiver parameters
 */
void ax_param_receiver_parameters(ax_config* config, ax_modulation* mod,
                                  ax_params* par)
{
  /* RX Bandwidth */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_ASK_COHERENT:
    case AX_MODULATION_PSK:
      par->rx_bandwidth = mod->bitrate; /* bitrate */
      break;

    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:     /* bitrate * (5/6 + m) */
      par->rx_bandwidth = mod->bitrate * ((5.0/6) + par->m);
      break;

    case AX_MODULATION_AFSK:    /* deviation?? */
      par->rx_bandwidth = mod->parameters.afsk.deviation; /* maybe?? */
      break;

    default:
      debug_printf("No clue about rx bandwidth for this mode.. Guessing\n");
      par->rx_bandwidth = 4*mod->bitrate; /* vague guess */
  }

  /* Baseband frequency */
  par->f_baseband = 5 * par->rx_bandwidth;
  debug_printf("f baseband = %d Hz\n", par->f_baseband);

  /* IF Frequency */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_ASK_COHERENT:
    case AX_MODULATION_PSK:
      par->if_frequency = 7000+mod->bitrate; /* guess?? */
      if (par->if_frequency < 9380) {
        par->if_frequency = 9380;     /* minimum 9380 Hz */
      }
      break;

    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK:
      par->if_frequency = (5 * par->rx_bandwidth) / 6;
      if (par->if_frequency < 3180) {
        par->if_frequency = 3180;     /* minimum 3180 Hz */
      }
      break;

    default:
      debug_printf("No clue about IF frequency for this mode.. Guessing\n");
      par->if_frequency = par->rx_bandwidth;
  }


  /* IF Frequency */
  par->iffreq = (uint32_t)((((float)par->if_frequency * config->f_xtaldiv *
                             (1 << 20)) / (float)config->f_xtal) + 0.5);
  debug_printf("IF frequency %d Hz = 0x%04x\n", par->if_frequency, par->iffreq);


  /* Decimation */
  par->decimation = (uint32_t)(((float)config->f_xtal /
                                (16.0 * config->f_xtaldiv * par->f_baseband)) + 0.5);
  if (par->decimation > 127) {
    par->decimation = 127;
    debug_printf("decimation capped at 127(!)\n");
  }
  debug_printf("decimation = %d\n", par->decimation);


  /* RX Data Rate */
  par->rx_data_rate = (uint32_t)((((float)config->f_xtal * 128) /
                                  ((float)config->f_xtaldiv * mod->bitrate *
                                   par->decimation)) + 0.5);
  debug_printf("rx data rate %d = 0x%04x\n", mod->bitrate, par->rx_data_rate);


  /* Max RF offset - Correct offset at first LO */
  if (mod->max_delta_carrier == 0) { /* not set */
    mod->max_delta_carrier = 1e3; /* 1kHz */
  }
  par->max_rf_offset = (uint32_t)((((float)mod->max_delta_carrier *
                                    (1 << 24)) / (float)config->f_xtal) + 0.5);
  debug_printf("max rf offset %d Hz = 0x%04x\n",
               mod->max_delta_carrier, par->max_rf_offset);


  /* Maximum deviation of FSK Demodulator */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK:
      par->fskd = (260 * par->m);   /* 260 provides a little wiggle room */
      par->fskd &= ~1;              /* clear LSB */
      debug_printf("min fsk demod dev 0x%04x\n", ~par->fskd & 0xFFFF);
      break;
    default:
      par->fskd = 0x80;         /* hardware default */
  }

  /* Bypass the Amplitude Lowpass filter */
  par->ampl_filter = 0;
}

/**
 * 5.15.10 afskctrl
 */
void ax_param_afskctrl(ax_config* config, ax_modulation* mod,
                       ax_params* par)
{
  /* Detector Bandwidth */
  float bw = (float)config->f_xtal /
    (32 * mod->bitrate * config->f_xtaldiv * par->decimation);

#ifdef USE_MATH_H
  par->afskshift = (uint8_t)(2 * log2(bw));
#else
  debug_printf("math.h required! define USE_MATH_H\n");
  par->afskshift = 4;                /* or define manually */
#endif

  debug_printf("afskshift (rx) %f = %d\n", bw, par->afskshift);
}

/**
 * 5.15.15+ receiver parameter sets
 */
enum ax_parameter_set_type {
  AX_PARAMETER_SET_INITIAL_SETTLING,
  AX_PARAMETER_SET_AFTER_PATTERN1,
  AX_PARAMETER_SET_DURING,
  AX_PARAMETER_SET_CONTINUOUS,
};
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

void ax_param_rx_parameter_set(ax_config* config, ax_modulation* mod,
                               ax_rx_param_set* pars, ax_params* par,
                               enum ax_parameter_set_type type)
{
  uint32_t tmg_corr_frac;
  uint32_t rffreq_gain_f;
  uint32_t rffreq_rg;


  /* AGC Gain Attack/Decay */
  /**
   * 0xFF freezes the AGC.  during preamble it's set for f_3dB of the
   * attack to be BITRATE, and f_3dB of the decay to be BITRATE/100
   */
  pars->agc_attack = ax_rx_agcgain(config, mod->bitrate); /* attack f_3dB: bitrate */
  pars->agc_decay = pars->agc_attack + 7; /* decay f_3dB: 128x slower */

  switch (type) {
    case AX_PARAMETER_SET_DURING: /* freeze AGC gain during packet */
      pars->agc_attack = pars->agc_decay = 0xF; break;
    case AX_PARAMETER_SET_CONTINUOUS: /* 4x slowdown compared to normal search */
      pars->agc_attack += 2;
      pars->agc_decay += 2;           /* fallthrough */
    default:
      /* limit attack > ~1kHz, decay > ~10Hz. could be relaxed?? */
      if (pars->agc_attack > 0x8) { pars->agc_attack = 0x8; }
      if (pars->agc_decay  > 0xE) { pars->agc_decay  = 0xE; }
      break;
  }
  debug_printf("agc gain: attack 0x%02x; decay 0x%02x\n",
               pars->agc_attack, pars->agc_decay);


  /* Gain of timing recovery loop */
  /**
   * TMGCORRFRAC - 4, 16, 32
   * tightning the loop...
   */
  switch (type) {
    case AX_PARAMETER_SET_INITIAL_SETTLING:
      tmg_corr_frac = 4;        /* fast lock */
      break;
    case AX_PARAMETER_SET_AFTER_PATTERN1:
      tmg_corr_frac = 16;
      break;
    default:
      tmg_corr_frac = 32;       /* low sampling time jitter */
      break;
  }
  pars->time_gain = (uint32_t)((float)par->rx_data_rate / tmg_corr_frac);
  if (pars->time_gain >= par->rx_data_rate - (1<<12)) { /* see 5.15.3 */
    /* effectively increase tmg_corr_frac to meet restriction */
    pars->time_gain = par->rx_data_rate - (1<<12);
    debug_printf("Had to limit time gain...\n");
  }
  debug_printf("time gain %d\n", pars->time_gain);


  /* Gain of datarate recovery loop */
  /**
   * TMGCORRFRAC - 256, 512, 1024
   * tightning the loop...
   */
  switch (type) {
    case AX_PARAMETER_SET_INITIAL_SETTLING:
      tmg_corr_frac = 256;        /* fast lock */
      break;
    case AX_PARAMETER_SET_AFTER_PATTERN1:
      tmg_corr_frac = 512;
      break;
    default:
      tmg_corr_frac = 1024;       /* low datarate jitter */
      break;
  }
  pars->dr_gain = (uint32_t)((float)par->rx_data_rate / tmg_corr_frac);
  debug_printf("datarate gain %d\n", pars->dr_gain);


  /* Gain of phase recovery loop / decimation filter fractional b/w */
  /**
   * Usually 0xC3. TODO ASK
   */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_PSK:     /* Maybe also PSK?? */
      /* TODO reduce decim fractional bandwidth when decimation reg overflows... */
      pars->filter_idx = 0x3;          /* decimation filter fractional bandwidth */
      pars->phase_gain = 0x0;          /* gain of the phase recovery loop */
      break;
    default:
      pars->filter_idx = 0x3;          /* decimation filter fractional bandwidth */
      pars->phase_gain = 0x3;          /* gain of the phase recovery loop */
      break;
  }

  /* Gain of Baseband frequency recovery loop */
  pars->baseband_rg_phase_det = 0xF; /* disable loop */
  pars->baseband_rg_freq_det = 0x1F; /* disable loop */

  /* Gain of RF frequency recovery loop */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK: /* not sure where this comes from, not documented */
      rffreq_gain_f = mod->bitrate;
      break;
    default:
      rffreq_gain_f = mod->bitrate * 4;
      break;
  }
  rffreq_rg = ax_rx_freqgain_rf_recovery_gain(config, rffreq_gain_f);

  switch (type) {
    case AX_PARAMETER_SET_DURING:
    case AX_PARAMETER_SET_CONTINUOUS:
      rffreq_rg += 4; /* 16x reduction in 'rffreq_gain_f' */
      break;
    default: break;
  }

  if (mod->fec) {
    rffreq_rg += 2;
  }

  /* limit to 13 */
  if (rffreq_rg > 0xD) { rffreq_rg = 0xD; }

  debug_printf("rffreq_recovery_gain 0x%02x\n", rffreq_rg);
  pars->rffreq_rg_phase_det = rffreq_rg;
  pars->rffreq_rg_freq_det = rffreq_rg;


  /* Amplitude Recovery Loop */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_PSK:     /* try to jump, averaging */
      pars->amplflags = AX_AMPLGAIN_TRY_TO_CORRECT_AMPLITUDE_ON_AGC_JUMP |
        AX_AMPLGAIN_AMPLITUDE_RECOVERY_AVERAGING;

      switch (type) {
        case AX_PARAMETER_SET_INITIAL_SETTLING:
        case AX_PARAMETER_SET_AFTER_PATTERN1:
          pars->amplgain = 2;         /* reduced gain */
          break;
        case AX_PARAMETER_SET_DURING:
        case AX_PARAMETER_SET_CONTINUOUS:
          pars->amplgain = 8;         /* increased gain */
          break;
      }

      break;
    default:           /* don't try to jump, peak det, default gain */
      pars->amplflags = AX_AMPLGAIN_AMPLITUDE_RECOVERY_PEAKDET;
      pars->amplgain = 6;
      break;
  }


  /* FSK Receiver Frequency Deviation */
  /**
   * Disable (0x00) for first pre-amble, then equal to deviation of signal???
   */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK:    /* also afsk?? */
      switch (type) {
        case AX_PARAMETER_SET_INITIAL_SETTLING:
        case AX_PARAMETER_SET_CONTINUOUS:
          pars->freq_dev = 0; break; /* disable to avoid locking at wrong offset */
        case AX_PARAMETER_SET_AFTER_PATTERN1:
        case AX_PARAMETER_SET_DURING:
          pars->freq_dev = (uint16_t)((par->m * 128 * 0.8) + 0.5); /* k_sf = 0.8 */
      }
      break;

    default:
      pars->freq_dev = 0;              /* no frequency deviation */
  }
  debug_printf("freqdev 0x%03x\n", pars->freq_dev);
}

/**
 * 5.20 packet format
 */
void ax_param_packet_format(ax_config* config, ax_modulation* mod, ax_params* par)
{
  (void)config;
  (void)mod;

  par->fec_sync_dis = 0;        /* don't disable fec sync search */
}

/**
 * 5.21 pattern match
 */
void ax_param_pattern_match(ax_config* config, ax_modulation* mod,
                            ax_params* par)
{
  (void)config;
  (void)mod;

  par->match1_threashold = 10;  /* maximum 15 */
  par->match0_threashold = 28;  /* maximum 31 */
}

/**
 * 5.22 packet controller
 */
void ax_param_packet_controller(ax_config* config, ax_modulation* mod,
                                ax_params* par)
{
  (void)config;

  par->pkt_misc_flags = 0;

  /* tx pll boost time */
  par->tx_pll_boost_time = 38;

  /* tx pll settle time */
  par->tx_pll_settle_time = 20;

  /* rx pll boost time */
  par->rx_pll_boost_time = 38;

  /* rx pll settle time */
  par->rx_pll_settle_time = 20;

  /* rx agc coarse  */
  par->rx_coarse_agc = 152;          /* 152 µs */

  /* rx agc */
  if (0) {                      /* TODO wake on radio */
    /* increasing settling time for narrow bandwidths */
    par->pkt_misc_flags |= AX_PKT_FLAGS_RSSI_UNITS_BIT_TIME;
    par->rx_agc_settling = 15;       /* guess */

    if (mod->fec) {
      par->rx_agc_settling *= 4;     /* 4 times for FEC */
    }
  } else {
    par->rx_agc_settling = 0;
  }

  /* rx rssi settling time */
  if (0) {                      /* TODO wake on radio */
    /* 3 bits time rssi setting time */
    par->pkt_misc_flags |= AX_PKT_FLAGS_RSSI_UNITS_BIT_TIME;
    par->rx_rssi_settling = 3;
  } else {
    /* 3us rssi setting time */
    par->pkt_misc_flags |= AX_PKT_FLAGS_RSSI_UNITS_MICROSECONDS;
    par->rx_rssi_settling = 3;
  }

  /* preamble 1 timeout */
  if (0) {                      /* TODO wake on radio */
    par->preamble_1_timeout = 25;
  } else {
    par->preamble_1_timeout = 0;
  }

  /* preamble 2 timeout */
  par->preamble_2_timeout = 23;      /* const. */

  /* rssi threashold = 221 */
  /**
   * 3log2(b/w) + x
   */
  par->rssi_abs_thr = 221;
}

/**
 * 5.26 'performance tuning'
 */
void ax_param_performace_tuning(ax_config* config, ax_modulation* mod,
                                ax_params* par)
{
  (void)config;
  (void)mod;

  par->perftuning_option = 0;
}

/**
 * populates ax_params structure
 */
void ax_populate_params(ax_config* config, ax_modulation* mod, ax_params* par)
{
  /* Modulation index for FSK modes */
  switch (mod->modulation & 0xf) {
    case AX_MODULATION_FSK:
      par->m = mod->parameters.fsk.modulation_index; break;
    case AX_MODULATION_MSK:
      par->m = 0.5; break;
    case AX_MODULATION_AFSK:
      par->m = 2 * (float)mod->parameters.afsk.deviation / mod->bitrate;
      break;
    default:
      par->m = 0;
  }

  ax_param_forward_error_correction(config, mod, par);
  ax_param_receiver_parameters(config, mod, par);
  ax_param_afskctrl(config, mod, par);

  /* receive sets */
  if (mod->continuous) {
    ax_param_rx_parameter_set(config, mod,
                              &par->rx_param_sets[3], par,
                              AX_PARAMETER_SET_CONTINUOUS);
  } else {
    ax_param_rx_parameter_set(config, mod,
                              &par->rx_param_sets[0], par,
                              AX_PARAMETER_SET_INITIAL_SETTLING);
    ax_param_rx_parameter_set(config, mod,
                              &par->rx_param_sets[1], par,
                              AX_PARAMETER_SET_AFTER_PATTERN1);
    ax_param_rx_parameter_set(config, mod,
                              &par->rx_param_sets[3], par,
                              AX_PARAMETER_SET_DURING);
  }

  ax_param_packet_format(config, mod, par);
  ax_param_pattern_match(config, mod, par);
  ax_param_packet_controller(config, mod, par);
  ax_param_performace_tuning(config, mod, par);

  par->is_params_set = 0x51;    /* yes, parameters are now set */
}
