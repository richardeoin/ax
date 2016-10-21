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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define USE_MATH_H
#ifdef USE_MATH_H
#include <math.h>
#endif

#include "ax/ax.h"
#include "ax/ax_hw.h"
#include "ax/ax_reg.h"
#include "ax/ax_reg_values.h"
#include "ax/ax_fifo.h"
#include "ax/ax_modes.h"
#include "ax/ax_calc.h"

#define TCXO 1

#include <stdio.h>
#define debug_printf printf

typedef struct ax_synthesiser_parameters {
  uint8_t loop, charge_pump_current;
} ax_synthesiser_parameters;

void ax_set_tx_power(ax_config* config, float power);
void ax_set_synthesiser_parameters(ax_config* config,
                                   ax_synthesiser_parameters* params,
                                   ax_synthesiser* synth,
                                   enum ax_vco_type vco_type);



/* __reentrantb void ax5043_set_registers_rxwor(void) __reentrant */
/* { */
/* 	AX_REG_TMGRXAGC                = 0x3E; */
/* 	AX_REG_TMGRXPREAMBLE1          = 0x19; */
/* 	AX_REG_PKTMISCFLAGS            = 0x03; */
/* } */


/* __reentrantb void ax5043_set_registers_rxcont(void) __reentrant */
/* { */
/* 	AX_REG_TMGRXAGC                = 0x00; */
/* 	AX_REG_TMGRXPREAMBLE1          = 0x00; */
/* 	AX_REG_PKTMISCFLAGS            = 0x00; */
/* } */


/* __reentrantb void ax5043_set_registers_rxcont_singleparamset(void) __reentrant */
/* { */
/* 	AX_REG_RXPARAMSETS             = 0xFF; */
/* 	AX_REG_FREQDEV13               = 0x00; */
/* 	AX_REG_FREQDEV03               = 0x00; */
/* 	AX_REG_AGCGAIN3                = 0xE8; */
/* } */




/**
 * FIFO -----------------------------------------------------
 */

/**
 * write tx data
 */
void ax_fifo_tx_data(ax_config* config, uint8_t* data, uint8_t length)
{
  uint8_t header[5];

  if (length < (256-3)) {       /* All in one go */

    /* wait for space */
    while (ax_hw_read_register_16(config, AX_REG_FIFOCOUNT) > (256-length));

    /* header */
    header[0] = AX_FIFO_CHUNK_DATA;
    header[1] = length+1;         /* incl flags */
    header[2] = AX_FIFO_TXDATA_PKTSTART |  AX_FIFO_TXDATA_PKTEND;

    ax_hw_write_fifo(config, header, 3);
    ax_hw_write_fifo(config, data, (uint8_t)length);

  } else {
    while(1);                   /* TODO longer packets */
  }
}
/**
 * read rx data
 */
uint16_t ax_fifo_rx_data(ax_config* config, ax_rx_chunk* chunk)
{
  uint8_t ptr[3];
  uint32_t scratch;

  uint8_t fifocount = ax_hw_read_register_16(config, AX_REG_FIFOCOUNT);
  if (fifocount == 0) {
    return 0;                   /* nothing to read */
  }

  debug_printf("got something. fifocount = %d\n", fifocount);

  chunk->chunk_t = ax_hw_read_register_8(config, AX_REG_FIFODATA);

  switch (chunk->chunk_t) {
    case AX_FIFO_CHUNK_DATA:
      ax_hw_read_register_bytes(config, AX_REG_FIFODATA, ptr, 2);

      chunk->chunk.data.length = ptr[0] - 1; /* not including flags here */
      chunk->chunk.data.flags  = ptr[1];

      /* read buffer */
      ax_hw_read_fifo(config,
                      chunk->chunk.data.data,
                      chunk->chunk.data.length + 1);

      return 3 + chunk->chunk.data.length;
      /* RSSI */
    case AX_FIFO_CHUNK_RSSI:
      /* 8-bit register value is always negative */
      chunk->chunk.rssi = 0xFF00 | ax_hw_read_register_8(config, AX_REG_FIFODATA);
      return 2;
      /* FREQOFFS */
    case AX_FIFO_CHUNK_FREQOFFS:
      chunk->chunk.freqoffs = ax_hw_read_register_16(config, AX_REG_FIFODATA);
      return 2;
      /* ANTRSSI 2 */
    case AX_FIFO_CHUNK_ANTRSSI2:
      ax_hw_read_register_bytes(config, AX_REG_FIFODATA, ptr, 2);

      chunk->chunk.antrssi2.rssi      = ptr[0];
      chunk->chunk.antrssi2.bgndnoise = ptr[1];
      return 3;
      /* TIMER */
    case AX_FIFO_CHUNK_TIMER:
      chunk->chunk.timer = ax_hw_read_register_24(config, AX_REG_FIFODATA);
      return 4;
      /* RFFREQOFFS */
    case AX_FIFO_CHUNK_RFFREQOFFS:
      scratch = ax_hw_read_register_24(config, AX_REG_FIFODATA);
      /* sign extend 24 -> 32 */
      chunk->chunk.rffreqoffs = (scratch & 0x800000) ?
        (0xFF000000 | scratch) : scratch;
      return 4;
      /* DATARATE */
    case AX_FIFO_CHUNK_DATARATE:
      chunk->chunk.datarate = ax_hw_read_register_24(config, AX_REG_FIFODATA);
      return 4;
      /* ANTRSSI3 */
    case AX_FIFO_CHUNK_ANTRSSI3:
      ax_hw_read_register_bytes(config, AX_REG_FIFODATA, ptr, 3);

      chunk->chunk.antrssi3.ant0rssi  = ptr[0];
      chunk->chunk.antrssi3.ant1rssi  = ptr[1];
      chunk->chunk.antrssi3.bgndnoise = ptr[2];
      return 4;
      /* default */
    default:
      return 1;
  }
}

/**
 * Clears the FIFO
 */
void ax_fifo_clear(ax_config* config)
{
  ax_hw_write_register_8(config, AX_REG_FIFOSTAT,
                         AX_FIFOCMD_CLEAR_FIFO_DATA_AND_FLAGS);
}
/**
 * Commits data written to the fifo
 */
void ax_fifo_commit(ax_config* config)
{
  ax_hw_write_register_8(config, AX_REG_FIFOSTAT,
                         AX_FIFOCMD_COMMIT);
}


/**
 * UTILITY FUNCTIONS ----------------------------------------
 */

/**
 * 5.1 revision and interface probing
 */
uint8_t ax_silicon_revision(ax_config* config)
{
  return ax_hw_read_register_8(config, AX_REG_SILICONREVISION);
}
/**
 * 5.1 revision and interface probing
 */
uint8_t ax_scratch(ax_config* config)
{
  return ax_hw_read_register_8(config, AX_REG_SCRATCH);
}

/**
 * 5.2 set operating mode
 */
void ax_set_pwrmode(ax_config* config, uint8_t pwrmode)
{
  config->pwrmode = pwrmode;
  ax_hw_write_register_8(config, AX_REG_PWRMODE, pwrmode); /* TODO R-m-w */
}



/**
 * 5.5 - 5.6 set modulation and fec parameters
 */
void ax_set_modulation_parameters(ax_config* config, ax_modulation* mod)
{
  /* modulation */
  ax_hw_write_register_8(config, AX_REG_MODULATION, mod->modulation);

  /* encoding (inv, diff, scram, manch..) */
  ax_hw_write_register_8(config, AX_REG_ENCODING, mod->encoding);

  /* framing */
  ax_hw_write_register_8(config, AX_REG_FRAMING, mod->framing);

  if ((mod->framing & 0xE) == AX_FRAMING_MODE_RAW_SOFT_BITS) {
    /* See 5.26 Performance Tuning */
    ax_hw_write_register_8(config, 0xF72, 0x06);
  } else {
    ax_hw_write_register_8(config, 0xF72, 0x00);
  }

  /* fec */
  if (mod->fec) {
    ax_hw_write_register_8(config, AX_REG_FEC, /* positive interleaver sync, 1/2 soft rx */
                           AX_FEC_POS | AX_FEC_ENA | (1 << 1));
  }
}

/**
 * Sets a PLL to a given frequency.
 *
 * returns the register value written
 */
uint32_t ax_set_freq_register(ax_config* config,
                              uint8_t reg, uint32_t frequency)
{
  uint32_t freq;

  /* we choose to always set the LSB to avoid spectral tones */
  freq = (uint32_t)(((double)frequency * (1 << 23)) /
                    (float)config->f_xtal);
  freq = (freq << 1) | 1;
  ax_hw_write_register_32(config, reg, freq);

  debug_printf("freq %d = 0x%08x\n", frequency, freq);

  return freq;
}
/**
 * 5.10 set synthesiser frequencies
 */
void ax_set_synthesiser_frequencies(ax_config* config)
{
  if (config->synthesiser.A.frequency) {
    /* FREQA */
    config->synthesiser.A.register_value =
      ax_set_freq_register(config,
                           AX_REG_FREQA, config->synthesiser.A.frequency);

  }
  if (config->synthesiser.B.frequency) {
    /* FREQB */
    config->synthesiser.B.register_value =
      ax_set_freq_register(config,
                           AX_REG_FREQB, config->synthesiser.B.frequency);
  }
}


/**
 * Synthesiser parameters for ranging
 */
ax_synthesiser_parameters synth_ranging = {
  /* Internal Loop Filter 100kHz */
  .loop = AX_PLLLOOP_FILTER_DIRECT | AX_PLLLOOP_INTERNAL_FILTER_BW_100_KHZ,
  /* Charge Pump I = 68uA */
  .charge_pump_current = 8,
};
/**
 * Synthesiser parameters for operation
 */
ax_synthesiser_parameters synth_operation = {
  /* Internal Loop Filter 500kHz */
  .loop = AX_PLLLOOP_FILTER_DIRECT | AX_PLLLOOP_INTERNAL_FILTER_BW_500_KHZ,
  /* Charge Pump I = 136uA */
  .charge_pump_current = 16,
};
/**
 * 5.10 set synthesiser parameters
 */
void ax_set_synthesiser_parameters(ax_config* config,
                                   ax_synthesiser_parameters* params,
                                   ax_synthesiser* synth,
                                   enum ax_vco_type vco_type)
{
  /* rfdiv */
  uint8_t vco_parameters =
    (synth->rfdiv == AX_RFDIV_1) ? AX_PLLVCODIV_RF_DIVIDER_DIV_TWO : 0;

  /* vco type */
  vco_parameters |= (vco_type & 0x30);

  /* set registers */
  ax_hw_write_register_8(config, AX_REG_PLLLOOP,   params->loop);
  ax_hw_write_register_8(config, AX_REG_PLLCPI,    params->charge_pump_current);
  ax_hw_write_register_8(config, AX_REG_PLLVCODIV, vco_parameters);

  /* f34 (See 5.26) */
  if (vco_parameters & AX_PLLVCODIV_RF_DIVIDER_DIV_TWO) {
    ax_hw_write_register_8(config, 0xF34, 0x28);
  } else {
    ax_hw_write_register_8(config, 0xF34, 0x08);
  }
}

/**
 * 5.15.8 - 5.15.10 set afsk receiver parameters
 */
void ax_set_afsk_rx_parameters(ax_config* config, ax_modulation* mod)
{
  uint16_t mark = mod->parameters.afsk.mark;
  uint16_t space = mod->parameters.afsk.space;
  uint16_t afskmark, afskspace;
  uint8_t afskshift;

  /* Mark */
  afskmark = (uint16_t)((((float)mark * (1 << 16) *
                          mod->decimation * config->f_xtaldiv) /
                         (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(config, AX_REG_AFSKMARK, afskmark);

  debug_printf("afskmark (rx) %d = 0x%04x\n", mark, afskmark);

  /* Space */
  afskspace = (uint16_t)((((float)space * (1 << 16) *
                           mod->decimation * config->f_xtaldiv) /
                          (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(config, AX_REG_AFSKSPACE, afskspace);

  debug_printf("afskspace (rx) %d = 0x%04x\n", space, afskspace);

  /* Detector Bandwidth */
  float bw = (float)config->f_xtal /
    (32 * mod->bitrate * config->f_xtaldiv * mod->decimation);

#ifdef USE_MATH_H
  afskshift = (uint8_t)(2 * log2(bw));
#else
  debug_printf("math.h required! define USE_MATH_H\n");
  afskshift = 4;                /* or define manually */
#endif

  ax_hw_write_register_16(config, AX_REG_AFSKCTRL, afskshift);

  debug_printf("afskshift (rx) %f = %d\n", bw, afskshift);
}
/**
 * 5.15 set receiver parameters
 */
void ax_set_rx_parameters(ax_config* config, ax_modulation* mod)
{
  uint32_t maxrfoffset;
  uint32_t rx_bandwidth;
  uint32_t if_frequency, iffreq;
  uint32_t f_baseband, decimation;
  uint32_t fskd;

  /* Modulation index for FSK modes */
  float m = 0.0;
  switch (mod->modulation) {
    case AX_MODULATION_FSK:
      m = mod->parameters.fsk.modulation_index; break;
    case AX_MODULATION_MSK:
      m = 0.5; break;
  }

  /* RX Bandwidth */
  switch (mod->modulation) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_ASK_COHERENT:
    case AX_MODULATION_PSK:
      rx_bandwidth = mod->bitrate; /* bitrate */
      break;

    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:     /* bitrate * (5/6 + m) */
      rx_bandwidth = mod->bitrate * ((5.0/6) + m);
      break;

    case AX_MODULATION_AFSK:    /* bitrate * (5/6 + dev?) */
      rx_bandwidth = mod->bitrate * /* maybe this works???? */
        ((5.0/6) + mod->parameters.afsk.deviation);
      break;

    default:
      debug_printf("No clue about rx bandwidth for this mode.. Guessing\n");
      rx_bandwidth = 4*mod->bitrate; /* vague guess */
  }

  /* Baseband frequency */
  f_baseband = 5 * rx_bandwidth;
  debug_printf("f baseband = %d Hz\n", f_baseband);

  /* IF Frequency */
  switch (mod->modulation) {
    case AX_MODULATION_ASK:
    case AX_MODULATION_ASK_COHERENT:
    case AX_MODULATION_PSK:
      if_frequency = 7000+mod->bitrate; /* guess?? */
      if (if_frequency < 9380) {
        if_frequency = 9380;     /* minimum 9380 Hz */
      }
      break;

    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK:
      if_frequency = (5 * rx_bandwidth) / 6;
      if (if_frequency < 3180) {
        if_frequency = 3180;     /* minimum 3180 Hz */
      }
      break;

    default:
      debug_printf("No clue about IF frequency for this mode.. Guessing\n");
      if_frequency = rx_bandwidth;
  }


  /* IF Frequency */
  iffreq = (uint32_t)((((float)if_frequency * config->f_xtaldiv *
                        (1 << 20)) / (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(config, AX_REG_IFFREQ, iffreq);
  debug_printf("IF frequency %d Hz = 0x%04x\n", if_frequency, iffreq);


  /* Decimation */
  decimation = (uint32_t)(((float)config->f_xtal /
                           (16.0 * config->f_xtaldiv * f_baseband)) + 0.5);
  if (decimation > 127) {
    decimation = 127;
    debug_printf("decimation capped at 127(!)\n");
  }
  mod->decimation = decimation;
  ax_hw_write_register_8(config, AX_REG_DECIMATION, mod->decimation);
  debug_printf("decimation = %d\n", decimation);


  /* RX Data Rate */
  mod->rxdatarate = (uint32_t)((((float)config->f_xtal * 128) /
                                ((float)config->f_xtaldiv * mod->bitrate *
                                 mod->decimation)) + 0.5);
  ax_hw_write_register_24(config, AX_REG_RXDATARATE, mod->rxdatarate);

  debug_printf("rx data rate %d = 0x%04x\n", mod->bitrate, mod->rxdatarate);


  /* Max Data Rate offset */
  ax_hw_write_register_24(config, AX_REG_MAXDROFFSET, 0x0);
  /* 0. Therefore < 1% */


  /* Max RF offset - Correct offset at first LO */
  maxrfoffset = (uint32_t)((((float)mod->max_delta_carrier *
                             (1 << 24)) / (float)config->f_xtal) + 0.5);
  ax_hw_write_register_24(config, AX_REG_MAXRFOFFSET,
                          AX_MAXRFOFFSET_FREQOFFSCORR_FIRST_LO | maxrfoffset);

  debug_printf("max rf offset %d Hz = 0x%04x\n",
               mod->max_delta_carrier, maxrfoffset);


  /* Maximum deviation of FSK Demodulator */
  switch (mod->modulation) {
    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
      fskd = (260 * m);         /* 260 provides a little wiggle room */
      fskd &= ~1;               /* clear LSB */
      ax_hw_write_register_16(config, AX_REG_FSKDMAX,  fskd & 0xFFFF);
      ax_hw_write_register_16(config, AX_REG_FSKDMIN, ~fskd & 0xFFFF);
      debug_printf("min fsk demod dev 0x%04x\n", ~fskd & 0xFFFF);
      break;
  }

  /* Bypass the Amplitude Lowpass filter */
  ax_hw_write_register_8(config, AX_REG_AMPLFILTER, 0x00);
}

/**
 * 5.15 Rx Parameter Sets
 */
enum ax_parameter_set_type {
  AX_PARAMETER_SET_INITIAL_SETTLING,
  AX_PARAMETER_SET_AFTER_PATTERN1,
  AX_PARAMETER_SET_DURING,
  AX_PARAMETER_SET_CONTINUOUS,
};
void ax_set_rx_parameter_set(ax_config* config,
                             ax_modulation* mod,
                             enum ax_parameter_set_type type)
{
  uint16_t ps;
  uint8_t agc_attack, agc_decay, agcgain;
  uint32_t tmg_corr_frac;
  uint32_t time_gain, dr_gain;
  uint8_t timegain, drgain;
  uint16_t freqdev;

  /* Modulation index for FSK modes */
  float m = 0.0;
  switch (mod->modulation) {
    case AX_MODULATION_FSK:
      m = mod->parameters.fsk.modulation_index; break;
    case AX_MODULATION_MSK:
      m = 0.5; break;
  }

  /* PARAMETER SET */
  switch (type) {
    case AX_PARAMETER_SET_INITIAL_SETTLING: /* use set 0 for initial settling */
      ps = AX_REG_RX_PARAMETER0; break;
    case AX_PARAMETER_SET_AFTER_PATTERN1: /* use set 1 after pattern 1 */
      ps = AX_REG_RX_PARAMETER1; break;
    case AX_PARAMETER_SET_DURING:
    case AX_PARAMETER_SET_CONTINUOUS: /* use set 3 during packet */
      ps = AX_REG_RX_PARAMETER3; break;
  }

  /* AGC Gain Attack/Decay */
  /**
   * 0xFF freezes the AGC.  during preamble it's set for f_3dB of the
   * attack to be BITRATE, and f_3dB of the decay to be BITRATE/100
   */
  agc_attack = ax_rx_agcgain(config, mod->bitrate); /* attack f_3dB: bitrate */
  agc_decay = agc_attack + 7;                       /* decay f_3dB: 128x slower */

  switch (type) {
    case AX_PARAMETER_SET_DURING: /* freeze AGC gain during packet */
      agc_attack = agc_decay = 0xF; break;
    case AX_PARAMETER_SET_CONTINUOUS: /* 4x slowdown compared to normal search */
      agc_attack += 2;
      agc_decay += 2;
      break;
    default: break;
  }
  /* limit attack > ~1kHz, decay > ~10Hz. could be relaxed?? */
  if (agc_attack > 0x8) { agc_attack = 0x8; }
  if (agc_decay  > 0xE) { agc_decay  = 0xE; }

  agcgain = ((agc_decay & 0xF) << 4) | (agc_attack & 0xF);
  ax_hw_write_register_8(config, ps + AX_RX_AGCGAIN, agcgain);


  /* AGC target value */
  /**
   * Always set to 132, which gives target output of 304 from 1023 counts
   */
  ax_hw_write_register_8(config, ps + AX_RX_AGCTARGET, 0x84);


  /* AGC digital threashold range */
  /**
   * Always set to zero, the analogue ADC always follows immediately
   */
  ax_hw_write_register_8(config, ps + AX_RX_AGCAHYST, 0x00);


  /* AGC minmax */
  /**
   * Always set to zero, this is probably best.
   */
  ax_hw_write_register_8(config, ps + AX_RX_AGCMINMAX, 0x00);


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
  time_gain = (uint32_t)((float)mod->rxdatarate / tmg_corr_frac);
  if (time_gain >= mod->rxdatarate - (1<<12)) { /* see 5.15.3 */
    /* effectively increase tmg_corr_frac to meet restriction */
    timegain = mod->rxdatarate - (1<<12);
  }
  timegain = ax_value_to_mantissa_exp_4_4(time_gain);
  debug_printf("time gain %d = 0x%02x\n", time_gain, timegain);
  ax_hw_write_register_8(config, ps + AX_RX_TIMEGAIN, timegain);


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
  dr_gain = (uint32_t)((float)mod->rxdatarate / tmg_corr_frac);
  drgain = ax_value_to_mantissa_exp_4_4(dr_gain);

  debug_printf("datarate gain %d = 0x%02x\n", dr_gain, drgain);
  ax_hw_write_register_8(config, ps + AX_RX_DRGAIN, drgain);


  /* Gain of phase recovery loop / decimation filter fractional b/w */
  /**
   * Usually 0xC3. TODO ASK
   */
  ax_hw_write_register_8(config, ps + AX_RX_PHASEGAIN, 0xC3);


  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINA, 0x0F);
  /* Always 0x0F (baseband frequency loop disabled) */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINB, 0x1F);
  /* Always 0x1F (baseband frequency loop disabled) */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINC, 0x0D);
  /* 0xB, 0xB, 0xD */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAIND, 0x0D);
  /* 0xB, 0xB, 0xD */

  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AMPLITUDEGAIN, 0x06);
  /* Always 0x6 */

  /* Receiver Frequency Deviation */
  /**
   * Disable (0x00) for first pre-amble, then equal to deviation of signal???
   */
  switch (mod->modulation) {
    case AX_MODULATION_FSK:
    case AX_MODULATION_MSK:
    case AX_MODULATION_AFSK:    /* also afsk?? */
      switch (type) {
        case AX_PARAMETER_SET_INITIAL_SETTLING:
        case AX_PARAMETER_SET_CONTINUOUS:
          freqdev = 0; break; /* disable to avoid locking at wrong offset */
        case AX_PARAMETER_SET_AFTER_PATTERN1:
        case AX_PARAMETER_SET_DURING:
          freqdev = (uint16_t)((m * 128 * 0.7) + 0.5); /* k_sf = 0.7 */
      }
      break;

    default:
      freqdev = 0;              /* no frequency deviation */
  }
  debug_printf("freqdev 0x%03x\n", freqdev);
  ax_hw_write_register_16(config, ps + AX_RX_FREQDEV, 0x0043);


  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_BBOFFSRES, 0x00);

}


/**
 * 5.15.8 - 5.15.9 set afsk transmit parameters
 */
void ax_set_afsk_tx_parameters(ax_config* config, ax_modulation* mod)
{
  uint16_t mark = mod->parameters.afsk.mark;
  uint16_t space = mod->parameters.afsk.space;
  uint16_t afskmark, afskspace;

  /* Mark */
  afskmark = (uint16_t)((((float)mark * (1 << 18)) /
                         (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(config, AX_REG_AFSKMARK, afskmark);

  debug_printf("afskmark (tx) %d = 0x%04x\n", mark, afskmark);

  /* Space */
  afskspace = (uint16_t)((((float)space * (1 << 18)) /
                          (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(config, AX_REG_AFSKSPACE, afskspace);

  debug_printf("afskspace (tx) %d = 0x%04x\n", space, afskspace);
}
/**
 * 5.16 set transmitter parameters
 *
 * * power - output power, as a fraction of maximum
 *
 * Pre-distortion is possible in hardware, but not supported here.
 */
void ax_set_tx_parameters(ax_config* config, ax_modulation* mod)
{
  uint16_t pwr;
  uint32_t deviation;
  uint32_t fskdev, txrate;

  /* frequency shaping mode of transmitter */
  switch (mod->modulation) {
    case AX_MODULATION_FSK:
      ax_hw_write_register_8(config, AX_REG_MODCFGF,
                             AX_MODCFGF_FREQSHAPE_GAUSSIAN_BT_0_5);
      break;
  }

  /* amplitude shaping mode of transmitter */
  ax_hw_write_register_8(config, AX_REG_MODCFGA,
                         AX_MODCFGA_TXDIFF | AX_MODCFGA_AMPLSHAPE_RAISED_COSINE);

  /* TX deviation */
  switch (mod->modulation) {
    case AX_MODULATION_PSK:     /* PSK */
      fskdev = 0;
      break;
    case AX_MODULATION_FSK:     /* FSK */

      deviation = (mod->parameters.fsk.modulation_index * 0.5 * mod->bitrate);

      fskdev = (uint32_t)((((float)deviation * (1 << 24)) /
                           (float)config->f_xtal) + 0.5);
      break;
    case AX_MODULATION_AFSK:    /* AFSK */

      deviation = mod->parameters.afsk.deviation;

      fskdev = (uint32_t)((((float)deviation * (1 << 24) * 0.858785) /
                           (float)config->f_xtal) + 0.5);
      break;
  }
  ax_hw_write_register_24(config, AX_REG_FSKDEV, fskdev);
  /* 0x2AB = 683. f_deviation = 666Hz... */
  debug_printf("fskdev %d = 0x%06x\n", deviation, fskdev);


  /* TX bitrate. We assume bitrate < f_xtal */
  txrate = (uint32_t)((((float)mod->bitrate * (1 << 24)) /
                       (float)config->f_xtal) + 0.5);
  ax_hw_write_register_24(config, AX_REG_TXRATE, txrate);

  debug_printf("bitrate %d = 0x%06x\n", mod->bitrate, txrate);

  /* check bitrate for asynchronous wire mode */
  if (1 && mod->bitrate >= config->f_xtal / 32) {
    debug_printf("for asynchronous wire mode, bitrate must be less than f_xtal/32\n");
  }

  /* TX power */
  pwr = (uint16_t)((mod->power * (1 << 12)) + 0.5);
  pwr = (pwr > 0xFFF) ? 0xFFF : pwr; /* max 0xFFF */
  ax_hw_write_register_16(config, AX_REG_TXPWRCOEFFB, pwr);

  debug_printf("power %f = 0x%03x\n", mod->power, pwr);
}

/**
 * 5.17 set PLL parameters
 */
void ax_set_pll_parameters(ax_config* config)
{
  uint8_t pllrngclk_div;

  /* VCO Current - 1250 uA VCO1, 250 uA VCO2 */
  ax_hw_write_register_8(config, AX_REG_PLLVCOI,
                         AX_PLLVCOI_ENABLE_MANUAL | 25);

  /* PLL Ranging Clock */
  pllrngclk_div = AX_PLLRNGCLK_DIV_2048;
  ax_hw_write_register_8(config, AX_REG_PLLRNGCLK, pllrngclk_div);
  /* approx 8kHz for 16MHz clock */

  config->f_pllrng = config->f_xtal / (1 << (8 + pllrngclk_div));
  /* TODO: config->f_pllrng should be less than 1/10 of the loop filter b/w */
  debug_printf("Ranging clock f_pllrng %d Hz\n", config->f_pllrng);
}
/**
 * 5.18 set xtal parameters
 */
void ax_set_xtal_parameters(ax_config* config)
{
  uint8_t xtalcap;
  uint8_t xtalosc;
  uint8_t xtalampl;
  uint8_t f35;

  /* Load Capacitance */
  if ((config->clock_source == AX_CLOCK_SOURCE_CRYSTAL) &&
      (config->load_capacitance != 0)) {

    if (config->load_capacitance == 3) {
      xtalcap = 0;
    } else if (config->load_capacitance == 8) {
      xtalcap = 1;
    } else if ((config->load_capacitance >= 9) &&
               (config->load_capacitance <= 39)) {
      xtalcap = (config->load_capacitance - 8) << 1;
    } else {
      debug_printf("xtal load capacitance %d not supported\n",
                   config->load_capacitance);
      xtalcap = 0;
    }

    ax_hw_write_register_8(config, AX_REG_XTALCAP, xtalcap);
  }

  /* Crystal Oscillator Control */
  if (config->f_xtal > 43*1000*1000) {
    xtalosc = 0x0D;             /* > 43 MHz */
  } else if (config->clock_source == AX_CLOCK_SOURCE_TCXO) {
    xtalosc = 0x04;             /* TCXO */
  } else {
    xtalosc = 0x03;             /*  */
  }
  ax_hw_write_register_8(config, AX_REG_XTALOSC, xtalosc);

  /* Crystal Oscillator Amplitude Control */
  if (config->clock_source == AX_CLOCK_SOURCE_TCXO) {
    xtalampl = 0x00;
  } else {
    xtalampl = 0x07;
  }
  ax_hw_write_register_8(config, AX_REG_XTALAMPL, xtalampl);

  /* F35 */
  if (config->f_xtal < 24800*1000) {
    f35 = 0x10;                 /* < 24.8 MHz */
    config->f_xtaldiv = 1;
  } else {
    f35 = 0x11;
    config->f_xtaldiv = 2;
  }
  ax_hw_write_register_8(config, 0xF35, f35);
}
/**
 * 5.19 set baseband parameters
 */
void ax_set_baseband_parameters(ax_config* config)
{
  ax_hw_write_register_8(config, AX_REG_BBTUNE, 0x0F);
  /* Baseband tuning value 0xF */

  ax_hw_write_register_8(config, AX_REG_BBOFFSCAP, 0x77);
  /* Offset capacitors all ones */
}
/**
 * 5.20 set packet format parameters
 */
void ax_set_packet_parameters(ax_config* config)
{
  ax_hw_write_register_8(config, AX_REG_PKTADDRCFG, 0x01);
  /* address at position 1 */

  ax_hw_write_register_8(config, AX_REG_PKTLENCFG, 0x80);
  /* 8 significant bits on length byte */

  ax_hw_write_register_8(config, AX_REG_PKTLENOFFSET, 0x00);
  /* zero offset on length byte */

  /* Maximum packet length */
  ax_hw_write_register_8(config, AX_REG_PKTMAXLEN, 0xC8);
  /* 0xC8 = 200 bytes */
}
/**
 * 5.21 set match parameters
 */
void ax_set_match_parameters(ax_config* config)
{
  /* match 1, then match 0 */

  ax_hw_write_register_32(config, AX_REG_MATCH0PAT, 0xAACCAACC);
  ax_hw_write_register_16(config, AX_REG_MATCH1PAT, 0x7E7E);

  ax_hw_write_register_8(config, AX_REG_MATCH1LEN, 0x8A);
  /* Raw received bits, 11-bit pattern */

  ax_hw_write_register_8(config, AX_REG_MATCH1MAX, 0x0A);
  /* signal a match if recevied bitstream matches for 10-bits or more */
}
/**
 * 5.22 set packet controller parameters
 */
void ax_set_packet_controller_parameters(ax_config* config)
{
  ax_hw_write_register_8(config, AX_REG_TMGTXBOOST, 0x33); /* 38us pll boost time */
  ax_hw_write_register_8(config, AX_REG_TMGTXSETTLE, 0x14); /* 20us tx pll settle time  */
  ax_hw_write_register_8(config, AX_REG_TMGRXBOOST, 0x33);  /* 38us rx pll boost time */
  ax_hw_write_register_8(config, AX_REG_TMGRXSETTLE, 0x14); /* 20us rx pll settle time */
  ax_hw_write_register_8(config, AX_REG_TMGRXOFFSACQ, 0x00); /* 0us bb dc offset aquis tim */
  ax_hw_write_register_8(config, AX_REG_TMGRXCOARSEAGC, 0x73); /* 152 us rx agc coarse  */
  ax_hw_write_register_8(config, AX_REG_TMGRXRSSI, 0x03);      /* 3us rssi setting time */
  ax_hw_write_register_8(config, AX_REG_TMGRXPREAMBLE2, 0x17); /* 23 bit preamble timeout */
  ax_hw_write_register_8(config, AX_REG_RSSIABSTHR, 0xDD);     /* rssi threashold = 221 */
  ax_hw_write_register_8(config, AX_REG_BGNDRSSITHR, 0x00);

  /* max chunk size = 240 bytes */
  ax_hw_write_register_8(config, AX_REG_PKTCHUNKSIZE,
                         AX_PKT_MAXIMUM_CHUNK_SIZE_240_BYTES);

  /* metadata to store */
  ax_hw_write_register_8(config, AX_REG_PKTSTOREFLAGS,
                         config->pkt_store_flags);

  /* accept multiple chunks */
  ax_hw_write_register_8(config, AX_REG_PKTACCEPTFLAGS,
                         0x3F);  //ALL!!! //AX_PKT_ACCEPT_MULTIPLE_CHUNKS);
}



/**
 * Wait for oscillator running and stable
 */
void ax_wait_for_oscillator(ax_config* config)
{
  int i = 0;
  while (!(ax_hw_read_register_8(config, AX_REG_XTALSTATUS) & 1)) {
    i++;
  }

  debug_printf("osc stable in %d cycles\n", i);
}



void ax5043_set_registers(ax_config* config, ax_modulation* mod)
{
  /* ax_hw_write_register_8(config, AX_REG_MODULATION, 0x08); */
  /* ax_hw_write_register_8(config, AX_REG_ENCODING, 0x00); */
  /* ax_hw_write_register_8(config, AX_REG_FRAMING, 0x24); */
  /* ax_hw_write_register_8(config, AX_REG_FEC, 0x13); */
  //
  ax_set_modulation_parameters(config, mod);

  ax_hw_write_register_8(config, AX_REG_PINFUNCSYSCLK, 0x01);
  ax_hw_write_register_8(config, AX_REG_PINFUNCDCLK, 0x01);
  ax_hw_write_register_8(config, AX_REG_PINFUNCDATA, 0x01);
  ax_hw_write_register_8(config, AX_REG_PINFUNCANTSEL, 0x01);
  ax_hw_write_register_8(config, AX_REG_PINFUNCPWRAMP, 0x07);
  ax_hw_write_register_8(config, AX_REG_WAKEUPXOEARLY, 0x01);

  /* ax_hw_write_register_16(config, AX_REG_IFFREQ, 0x00C8); */
  /* ax_hw_write_register_8(config, AX_REG_DECIMATION, 0x44); */
  /* ax_hw_write_register_24(config, AX_REG_RXDATARATE, 0x003C2E); */
  /* ax_hw_write_register_24(config, AX_REG_MAXDROFFSET, 0x0); */
  /* ax_hw_write_register_24(config, AX_REG_MAXRFOFFSET, 0x80037B); */
  /* ax_hw_write_register_16(config, AX_REG_FSKDMAX, 0x00A6); */
  /* ax_hw_write_register_16(config, AX_REG_FSKDMIN, 0xFF5A); */
  /* ax_hw_write_register_8(config, AX_REG_AMPLFILTER, 0x00); */
  //
  ax_set_rx_parameters(config, mod);


  ax_hw_write_register_8(config, AX_REG_RXPARAMSETS, 0xF4);
  /* 0, 1, 3, 3 */

  /* RX 0 */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_AGCGAIN, 0xD7);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_TIMEGAIN, 0xF8);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_DRGAIN, 0xF2);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINC, 0x0B);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAIND, 0x0B);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(config, AX_REG_RX_PARAMETER0 + AX_RX_FREQDEV, 0x0000);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER0 + AX_RX_BBOFFSRES, 0x00);
  /* RX 1 */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_AGCGAIN, 0xD7);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_AGCAHYST, 0x00);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_AGCMINMAX, 0x00);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_TIMEGAIN, 0xF6);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_DRGAIN, 0xF1);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINC, 0x0B);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAIND, 0x0B);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQDEV, 0x0043);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER1 + AX_RX_BBOFFSRES, 0x00);
  /* RX 3 */
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AGCGAIN, 0xFF);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AGCAHYST, 0x00);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AGCMINMAX, 0x00);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_TIMEGAIN, 0xF5);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_DRGAIN, 0xF0);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINC, 0x0D);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAIND, 0x0D);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(config, AX_REG_RX_PARAMETER1 + AX_RX_FREQDEV, 0x0043);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(config, AX_REG_RX_PARAMETER3 + AX_RX_BBOFFSRES, 0x00);
  ax_set_rx_parameter_set(config, mod, AX_PARAMETER_SET_DURING);

  /* ax_hw_write_register_8(config, AX_REG_MODCFGF, 0x03); */
  /* ax_hw_write_register_24(config, AX_REG_FSKDEV, 0x0002AB); */
  /* ax_hw_write_register_8(config, AX_REG_MODCFGA, 0x05); */
  /* ax_hw_write_register_24(config, AX_REG_TXRATE, 0x000802); */
  /* ax_hw_write_register_16(config, AX_REG_TXPWRCOEFFB, 0x0FFF); */
  //
  ax_set_tx_parameters(config, mod);

  /* ax_hw_write_register_8(config, AX_REG_PLLVCOI, 0x99); */
  /* ax_hw_write_register_8(config, AX_REG_PLLRNGCLK, 0x03); */
  //
  ax_set_pll_parameters(config);

  /* ax_hw_write_register_8(config, AX_REG_BBTUNE, 0x0F); */
  /* ax_hw_write_register_8(config, AX_REG_BBOFFSCAP, 0x77); */
  //
  ax_set_baseband_parameters(config);

  /* ax_hw_write_register_8(config, AX_REG_PKTADDRCFG, 0x01); */
  /* ax_hw_write_register_8(config, AX_REG_PKTLENCFG, 0x80); */
  /* ax_hw_write_register_8(config, AX_REG_PKTLENOFFSET, 0x00); */
  /* ax_hw_write_register_8(config, AX_REG_PKTMAXLEN, 0xC8); */
  //
  ax_set_packet_parameters(config);

  /* ax_hw_write_register_32(config, AX_REG_MATCH0PAT, 0xAACCAACC); */
  /* ax_hw_write_register_16(config, AX_REG_MATCH1PAT, 0x7E7E); */
  /* ax_hw_write_register_8(config, AX_REG_MATCH1LEN, 0x8A); */
  /* ax_hw_write_register_8(config, AX_REG_MATCH1MAX, 0x0A); */
  //
  ax_set_match_parameters(config);

  /* Packet controller */
  /* ax_hw_write_register_8(config, AX_REG_TMGTXBOOST, 0x33); */
  /* ax_hw_write_register_8(config, AX_REG_TMGTXSETTLE, 0x14); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXBOOST, 0x33); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXSETTLE, 0x14); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXOFFSACQ, 0x00); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXCOARSEAGC, 0x73); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXRSSI, 0x03); */
  /* ax_hw_write_register_8(config, AX_REG_TMGRXPREAMBLE2, 0x17); */
  /* ax_hw_write_register_8(config, AX_REG_RSSIABSTHR, 0xDD); */
  /* ax_hw_write_register_8(config, AX_REG_BGNDRSSITHR, 0x00); */
  /* ax_hw_write_register_8(config, AX_REG_PKTCHUNKSIZE, 0x0D); */
  /* ax_hw_write_register_8(config, AX_REG_PKTACCEPTFLAGS, 0x20); */
  //
  ax_set_packet_controller_parameters(config);

  ax_hw_write_register_8(config, AX_REG_DACCONFIG, 0x00);
  ax_hw_write_register_8(config, AX_REG_REF, 0x03);



  ax_hw_write_register_8(config, 0xF1C, 0x07); /* const */
  ax_hw_write_register_8(config, 0xF21, 0x68); /* !! */
  ax_hw_write_register_8(config, 0xF22, 0xFF); /* !! */
  ax_hw_write_register_8(config, 0xF23, 0x84); /* !! */
  ax_hw_write_register_8(config, 0xF26, 0x98); /* !! */



  ax_hw_write_register_8(config, 0xF44, 0x25); /* !! */
  //ax_hw_write_register_8(config, 0xF44, 0x24); /* !! */
}


void ax5043_set_registers_tx(ax_config* config)
{
  ax_set_synthesiser_parameters(config,
                                &synth_operation,
                                &config->synthesiser.A,
                                config->synthesiser.vco_type);

  ax_hw_write_register_8(config, 0xF00, 0x0F); /* const */
  ax_hw_write_register_8(config, 0xF18, 0x06); /* ?? */
}


void ax5043_set_registers_rx(ax_config* config)
{
  ax_set_synthesiser_parameters(config,
                                &synth_operation,
                                &config->synthesiser.A,
                                config->synthesiser.vco_type);

  ax_hw_write_register_8(config, 0xF00, 0x0F); /* const */
  ax_hw_write_register_8(config, 0xF18, 0x02); /* ?? */
}


/**
 * MAJOR FUNCTIONS ------------------------------------------
 */

/**
 * Performs a ranging operation
 *
 * updates values in synth structure
 */
void ax_do_vco_ranging(ax_config* config,
                       uint16_t pllranging,
                       ax_synthesiser* synth,
                       enum ax_vco_type vco_type)
{
  uint8_t r;

  /* set vco range (VCOR) to 8 if unknown */
  synth->vco_range = (synth->vco_range_known == 0) ? 8 : synth->vco_range;

  /* set rf div (RFDIV) if unknown */
  if (synth->rfdiv == AX_RFDIV_UKNOWN) {
    synth->rfdiv = (synth->frequency < 525*1000*1000) ? AX_RFDIV_1 : AX_RFDIV_0;
  }

  /* Set default 100kHz loop BW for ranging */
  ax_set_synthesiser_parameters(config, &synth_ranging, synth, vco_type);


  /* Set RNGSTART bit (PLLRANGINGA,B) */
  ax_hw_write_register_8(config, pllranging,
                         synth->vco_range | AX_PLLRANGING_RNG_START);

  /* Wait for RNGSTART bit to clear */
  do {
    r = ax_hw_read_register_8(config, pllranging);
  } while (r & AX_PLLRANGING_RNG_START);

  /* Check RNGERR bit */
  if (r & AX_PLLRANGING_RNGERR) {
    /* ranging error */
    debug_printf("Ranging error!\n");
  } else {
    debug_printf("Ranging done\n");
  }
  debug_printf("r = 0x%02x\n", r);

  /* Update vco_range */
  synth->vco_range = r & 0xF;
  synth->vco_range_known = 1;
}
/**
 * Ranges both VCOs
 *
 * re-ranging is required for > 5MHz in 868/915 or > 2.5MHz in 433
 */
void ax_vco_ranging(ax_config* config)
{
  debug_printf("starting vco ranging...\n");

  /* Enable TCXO if used */
  if (config->tcxo_enable) { config->tcxo_enable(); }

  /* Set PWRMODE to STANDBY */
  ax_set_pwrmode(config, AX_PWRMODE_STANDBY);

  /* Set FREQA,B registers to correct value */
  ax_set_synthesiser_frequencies(config);

  /* Manual VCO current, 27 = 1350uA VCO1, 270uA VCO2 */
  ax_hw_write_register_8(config, AX_REG_PLLVCOI,
                         AX_PLLVCOI_ENABLE_MANUAL | 27);

  /* Wait for oscillator to be stable */
  ax_wait_for_oscillator(config);

  /* do ranging */
  ax_do_vco_ranging(config, AX_REG_PLLRANGINGA,
                    &config->synthesiser.A, config->synthesiser.vco_type);
  ax_do_vco_ranging(config, AX_REG_PLLRANGINGB,
                    &config->synthesiser.B, config->synthesiser.vco_type);


  /* Set PWRMODE to POWERDOWN */
  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  /* Disable TCXO if used */
  if (config->tcxo_disable) { config->tcxo_disable(); }
}

/**
 * PUBLIC FUNCTIONS ------------------------------------------
 */

void ax_tx_on(ax_config* config, ax_modulation* mod)
{
  debug_printf("going for transmit...\n");

  ax5043_set_registers(config, mod);
  ax5043_set_registers_tx(config);    /* set tx registers??? */

  /* Enable TCXO if used */
  if (config->tcxo_enable) { config->tcxo_enable(); }

  /* Place chip in FULLTX mode */
  ax_set_pwrmode(config, AX_PWRMODE_FULLTX);

  /* Wait for oscillator to start running  */
  ax_wait_for_oscillator(config);

  /* Set PWRMODE to POWERDOWN */
  //ax_set_pwrmode(AX_PWRMODE_DEEPSLEEP);

  /* Disable TCXO if used */
  if (config->tcxo_disable) { config->tcxo_disable(); }
}

/**
 * Loads packet into the FIFO for transmission
 */
void ax_tx_packet(ax_config* config, uint8_t* packet, uint16_t length)
{
  if (config->pwrmode != AX_PWRMODE_FULLTX) {
    debug_printf("PWRMODE must be FULLTX before writing to FIFO!\n");
    return;
  }

  /* Ensure the SVMODEM bit (POWSTAT) is set high (See 3.1.1) */
  while (!(ax_hw_read_register_8(config, AX_REG_POWSTAT) & AX_POWSTAT_SVMODEM));

  /* Write preamble and packet to the FIFO */
  ax_fifo_tx_data(config, packet, length);

  /* Commit FIFO contents */
  ax_fifo_commit(config);

  debug_printf("packet committed to FIFO!\n");
}

/**
 * First attempt at receiver, don't care about power
 */
void ax_rx_on(ax_config* config, ax_modulation* mod)
{
  ax_rx_chunk rx_chunk;
  float offset;

  /* TODO set defaults in modulation structure  */
  mod->max_delta_carrier = 2*870; // 2*2ppm TODO ppm calculations

  /* Meta-data can be automatically added to FIFO, see PKTSTOREFLAGS */

  /* Place chip in FULLRX mode */
  ax5043_set_registers(config, mod);
  ax_set_pwrmode(config, AX_PWRMODE_FULLRX);

  ax5043_set_registers_rx(config);    /* set rx registers??? */

  /* Enable TCXO if used */
  if (config->tcxo_enable) { config->tcxo_enable(); }

  /* Clear FIFO */
  ax_fifo_clear(config);

  while (1) {
    /* Check if FIFO is not empty */
    if (ax_fifo_rx_data(config, &rx_chunk)) {

      /* Got something from FIFO */
      switch (rx_chunk.chunk_t) {
        case AX_FIFO_CHUNK_DATA:

          debug_printf("flags 0x%02x\n", rx_chunk.chunk.data.flags);
          debug_printf("length %d\n", rx_chunk.chunk.data.length);

          switch (mod->framing & 0x7) {
            default:              /* anything else, unsure */
              /* print byte-by-byte */
              for (int i = 0; i < rx_chunk.chunk.data.length; i++) {
                debug_printf("data %d: 0x%02x %c\n", i,
                             rx_chunk.chunk.data.data[i+1],
                             rx_chunk.chunk.data.data[i+1]);
              }
          }

          break;

        case AX_FIFO_CHUNK_RSSI:
          debug_printf("rssi %d dB\n", rx_chunk.chunk.rssi);
          break;

        case AX_FIFO_CHUNK_RFFREQOFFS:
          debug_printf("rf offset %d Hz\n", rx_chunk.chunk.rffreqoffs);
          break;

        case AX_FIFO_CHUNK_FREQOFFS:
          offset = rx_chunk.chunk.freqoffs * 2000;
          debug_printf("freq offset %f \n", offset / (1 << 16));
          break;

        default:
          debug_printf("some other chunk type 0x%02x\n", rx_chunk.chunk_t);
          break;
      }
    }
  }

  /* Disable TCXO if used */
  if (config->tcxo_disable) { config->tcxo_disable(); }
}

/**
 * Waits for any ongoing operations to complete, and then shuts down the radio
 */
void ax_off(ax_config* config)
{
  /* Wait for any ongoing operations to complete by polling RADIOSTATE */
  while ((ax_hw_read_register_8(config, AX_REG_RADIOSTATE) & 0xF) > 1) {
    /* Not IDLE or POWERDOWN */
  }

  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  debug_printf("ax_off complete!\n");
}

/**
 * Initialise radio.
 *
 * * reset
 * * check SPI interface functions
 * * range VCOs
 */
int ax_init(ax_config* config)
{
  /* must set spi_transfer */
  if (!config->spi_transfer) {
    return AX_INIT_SET_SPI;
  }

  /* Scratch */
  uint8_t scratch = ax_scratch(config);
  debug_printf("Scratch 0x%X\n", scratch);

  if (scratch != AX_SCRATCH) {
    debug_printf("Bad scratch value.\n");

    return AX_INIT_BAD_SCRATCH;
  }

  /* Revision */
  uint8_t silicon_revision = ax_silicon_revision(config);
  debug_printf("Silcon Revision 0x%X\n", silicon_revision);

  if (silicon_revision != AX_SILICONREVISION) {
    debug_printf("Bad Silcon Revision value.\n");

    return AX_INIT_BAD_REVISION;
  }



  /* Reset the chip */

  /* Set SEL high for at least 1us, then low */

  /* Wait for MISO to go high */

  /* Set RST bit (PWRMODE) */
  ax_hw_write_register_8(config, AX_REG_PWRMODE, AX_PWRMODE_RST);

  /* AX is now in reset */

  /* Set the PWRMODE register to POWERDOWN, also clears RST bit */
  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  /* Program parameters.. (these could initially come from windows software, or be calculated) */
  ax_set_xtal_parameters(config);

  /* Perform auto-ranging for both VCOs */
  ax_vco_ranging(config);


  return AX_INIT_OK;
}
