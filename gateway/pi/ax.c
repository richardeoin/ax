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

#include "ax.h"
#include "ax_hw.h"
#include "ax_reg.h"
#include "ax_reg_values.h"
#include "ax_fifo.h"
#include "ax_modes.h"

#define TCXO 1

#include <stdio.h>
#define debug_printf printf

typedef struct ax_synthesiser_parameters {
  uint8_t loop, charge_pump_current;
} ax_synthesiser_parameters;

void ax_set_tx_power(float power);
void ax_set_synthesiser_parameters(ax_synthesiser_parameters* params,
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
void ax_fifo_tx_data(uint8_t* data, uint8_t length)
{
  /* TODO wait for free space in the FIFO if needed */

  uint8_t header[5];

  header[0] = AX_FIFO_CHUNK_DATA;
  header[1] = 2+1;              /* incl flags */
  header[2] = AX_FIFO_TXDATA_PKTSTART | AX_FIFO_TXDATA_UNENC;
  header[3] = 0x7E;
  header[4] = 0x7E;             /* preamble */

  ax_hw_write_fifo(0, header, 5);

  header[0] = AX_FIFO_CHUNK_DATA;
  header[1] = length+1;         /* incl flags */
  header[2] = AX_FIFO_TXDATA_PKTEND;

  ax_hw_write_fifo(0, header, 3);
  ax_hw_write_fifo(0, data, (uint8_t)length);
}
/**
 * read rx data
 */
uint16_t ax_fifo_rx_data(int channel, ax_rx_chunk* chunk)
{
  uint8_t ptr[3];

  if (ax_hw_read_register_16(channel, AX_REG_FIFOCOUNT) == 0) {
    return 0;                   /* nothing to read */
  }

  chunk->chunk_t = ax_hw_read_register_8(channel, AX_REG_FIFODATA);

  switch (chunk->chunk_t) {
    case AX_FIFO_CHUNK_DATA:
      ax_hw_read_register_bytes(channel, AX_REG_FIFODATA, ptr, 2);

      chunk->chunk.data.length = ptr[0] - 1; /* not including flags here */
      chunk->chunk.data.flags  = ptr[1];

      /* read buffer */
      ax_hw_read_fifo(channel,
                      chunk->chunk.data.data,
                      chunk->chunk.data.length + 1);

      return 3 + chunk->chunk.data.length;
                                /* RSSI */
    case AX_FIFO_CHUNK_RSSI:
      chunk->chunk.rssi = ax_hw_read_register_8(channel, AX_REG_FIFODATA);
      return 2;
                                /* FREQOFFS */
    case AX_FIFO_CHUNK_FREQOFFS:
      chunk->chunk.freqoffs = ax_hw_read_register_16(channel, AX_REG_FIFODATA);
      return 2;
                                /* ANTRSSI 2 */
    case AX_FIFO_CHUNK_ANTRSSI2:
      ax_hw_read_register_bytes(channel, AX_REG_FIFODATA, ptr, 2);

      chunk->chunk.antrssi2.rssi      = ptr[0];
      chunk->chunk.antrssi2.bgndnoise = ptr[1];
      return 3;
                                /* TIMER */
    case AX_FIFO_CHUNK_TIMER:
      chunk->chunk.timer = ax_hw_read_register_24(channel, AX_REG_FIFODATA);
      return 4;
                                /* RFFREQOFFS */
    case AX_FIFO_CHUNK_RFFREQOFFS:
      chunk->chunk.rffreqoffs = ax_hw_read_register_24(channel, AX_REG_FIFODATA);
      return 4;
                                /* DATARATE */
    case AX_FIFO_CHUNK_DATARATE:
      chunk->chunk.datarate = ax_hw_read_register_24(channel, AX_REG_FIFODATA);
      return 4;
                                /* ANTRSSI3 */
    case AX_FIFO_CHUNK_ANTRSSI3:
      ax_hw_read_register_bytes(channel, AX_REG_FIFODATA, ptr, 3);

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
void ax_fifo_clear(void)
{
  ax_hw_write_register_8(0, AX_REG_FIFOSTAT,
                         AX_FIFOCMD_CLEAR_FIFO_DATA_AND_FLAGS);
}
/**
 * Commits data written to the fifo
 */
void ax_fifo_commit(void)
{
  ax_hw_write_register_8(0, AX_REG_FIFOSTAT,
                         AX_FIFOCMD_COMMIT);
}


/**
 * UTILITY FUNCTIONS ----------------------------------------
 */

/**
 * 5.2 set operating mode
 */
void ax_set_pwrmode(ax_config* config, uint8_t pwrmode)
{
  config->pwrmode = pwrmode;
  ax_hw_write_register_8(0, AX_REG_PWRMODE, pwrmode); /* TODO R-m-w */
}



/**
 * 5.5 - 5.6 set modulation and fec parameters
 */
void ax_set_modulation_parameters(ax_config* config, ax_modulation* mod)
{
  /* modulation */
  ax_hw_write_register_8(0, AX_REG_MODULATION, mod->modulation);

  /* encoding (inv, diff, scram, manch..) */
  ax_hw_write_register_8(0, AX_REG_ENCODING, mod->encoding);

  /* framing */
  ax_hw_write_register_8(0, AX_REG_FRAMING,
                         AX_FRAMING_MODE_HDLC | AX_FRAMING_CRCMODE_CRC_16);

  /* fec */
  if (mod->fec) {
    ax_hw_write_register_8(0, AX_REG_FEC, /* positive interleaver sync, 1/2 soft rx */
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
  ax_hw_write_register_32(0, reg, freq);

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
void ax_set_synthesiser_parameters(ax_synthesiser_parameters* params,
                                   ax_synthesiser* synth,
                                   enum ax_vco_type vco_type)
{
  /* rfdiv */
  uint8_t vco_parameters =
    (synth->rfdiv == AX_RFDIV_1) ? AX_PLLVCODIV_RF_DIVIDER_DIV_TWO : 0;

  /* TODO vco type */
  (void)vco_type;
  vco_parameters |=
    AX_PLLVCODIV_RF_FULLY_INTERNAL_VCO1 |
    AX_PLLVCODIV_RF_INTERNAL_VCO2_EXTERNAL_INDUCTOR;

  /* set registers */
  ax_hw_write_register_8(0, AX_REG_PLLLOOP,   params->loop);
  ax_hw_write_register_8(0, AX_REG_PLLCPI,    params->charge_pump_current);
  ax_hw_write_register_8(0, AX_REG_PLLVCODIV, vco_parameters);

  /* f34 (See 5.26) */
  if (vco_parameters & AX_PLLVCODIV_RF_DIVIDER_DIV_TWO) {
    ax_hw_write_register_8(0, 0xF34, 0x28);
  } else {
    ax_hw_write_register_8(0, 0xF34, 0x08);
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
                          config->decimation * config->f_xtaldiv) /
                         (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(0, AX_REG_AFSKMARK, afskmark);

  debug_printf("afskmark (rx) %d = 0x%04x\n", mark, afskmark);

  /* Space */
  afskspace = (uint16_t)((((float)space * (1 << 16) *
                           config->decimation * config->f_xtaldiv) /
                          (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(0, AX_REG_AFSKSPACE, afskspace);

  debug_printf("afskspace (rx) %d = 0x%04x\n", space, afskspace);

  /* Detector Bandwidth */
  float bw = (float)config->f_xtal /
    (32 * mod->bitrate * config->f_xtaldiv * config->decimation);

#ifdef USE_MATH_H
  afskshift = (uint8_t)(2 * log2(bw));
#else
  debug_printf("math.h required! define USE_MATH_H\n");
  afskshift = 4;                /* or define manually */
#endif

  ax_hw_write_register_16(0, AX_REG_AFSKCTRL, afskshift);

  debug_printf("afskshift (rx) %f = %d\n", bw, afskshift);
}
/**
 * 5.15 set receiver parameters
 */
void ax_set_rx_parameters(ax_config* config, ax_modulation* mod)
{
  uint32_t rxdatarate, maxrfoffset;

  /* IF Frequency */
  ax_hw_write_register_16(0, AX_REG_IFFREQ, 0x00C8);
  /* 0xC8 = 200. Therefore f_IF = 3122 Hz */

  /* Decimation */
  ax_hw_write_register_8(0, AX_REG_DECIMATION, 0x44);
  config->decimation = 0x44;
  /* 68x decimation */


  /* RX Data Rate */
  rxdatarate = (uint32_t)((((float)config->f_xtal * 128) /
                           ((float)config->f_xtaldiv * mod->bitrate *
                            config->decimation)) + 0.5);
  ax_hw_write_register_24(0, AX_REG_RXDATARATE, rxdatarate);

  debug_printf("rx data rate %d = 0x%04x\n", mod->bitrate, rxdatarate);


  /* Max Data Rate offset */
  ax_hw_write_register_24(0, AX_REG_MAXDROFFSET, 0x0);
  /* 0. Therefore < 1% */


  /* Max RF offset - Correct offset at first LO */
  maxrfoffset = (uint32_t)((((float)mod->max_delta_carrier *
                             (1 << 24)) / (float)config->f_xtal) + 0.5);
  ax_hw_write_register_24(0, AX_REG_MAXRFOFFSET,
                          AX_MAXRFOFFSET_FREQOFFSCORR_FIRST_LO | maxrfoffset);

  debug_printf("max rf offset %d = 0x%04x\n",
               mod->max_delta_carrier, maxrfoffset);


  /* FSK Deviation */
  ax_hw_write_register_16(0, AX_REG_FSKDMAX, 0x00A6);
  ax_hw_write_register_16(0, AX_REG_FSKDMIN, 0xFF5A);
  /* 0xA6 = 166. In Manual Mode??? Only for 4FSK??? */

  /* Bypass the Amplitude Lowpass filter */
  ax_hw_write_register_8(0, AX_REG_AMPLFILTER, 0x00);
}

/**
 * Work in progress
 */
void ax_set_rx_parameter_set()
{
  /* AGC Gain Attack/Decay */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCGAIN, 0xFF);
  /**
   * 0xFF freezes the ADC.  during preamble it's set for f_3dB of the
   * attack to be BITRATE, and f_3dB of the decay to be BITRATE/100
   */
  /* AGC target value */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCTARGET, 0x84);
  /**
   * Always set to 132, which gives target output of 304 from 1023 counts
   */

  /* ADC digital threashold range */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCAHYST, 0x00);
  /**
   * Always set to zero, the analogue ADC always follows immediately
   */

  /* AGC minmax */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCMINMAX, 0x00);
  /**
   * Always set to zero, this is probably best
   */

  /* Gain of timing recovery loop */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_TIMEGAIN, 0xF5);
  /**
   * Values - 0xF8, 0xF6, 0xF5
   * TMGCORRFRAC - 4, 16, 32
   * tightning the loop...
   */

  /* Gain of datarate recovery loop */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_DRGAIN, 0xF0);
  /**
   * Values - 0xF2, 0xF1, 0xF0
   * TMGCORRFRAC - 256, 512, 1024
   * tightning the loop...
   */

  /* Gain of phase recovery look / decimation filter fractional b/w */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_PHASEGAIN, 0xC3);
  /**
   * Always 0xC3
   */

  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINA, 0x0F);
  /* Always 0x0F (baseband frequency loop disabled) */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINB, 0x1F);
  /* Always 0x1F (baseband frequency loop disabled) */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINC, 0x0D);
  /* 0xB, 0xB, 0xD */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAIND, 0x0D);
  /* 0xB, 0xB, 0xD */

  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AMPLITUDEGAIN, 0x06);
  /* Always 0x6 */

  /* Receiver Frequency Deviation */
  ax_hw_write_register_16(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQDEV, 0x0043);
  /**
   * Disable (0x00) for first pre-amble, then equal to deviation of signal???
   */

  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_BBOFFSRES, 0x00);

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
  ax_hw_write_register_16(0, AX_REG_AFSKMARK, afskmark);

  debug_printf("afskmark (tx) %d = 0x%04x\n", mark, afskmark);

  /* Space */
  afskspace = (uint16_t)((((float)space * (1 << 18)) /
                          (float)config->f_xtal) + 0.5);
  ax_hw_write_register_16(0, AX_REG_AFSKSPACE, afskspace);

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
  ax_hw_write_register_8(0, AX_REG_MODCFGF,
                         AX_MODCFGF_FREQSHAPE_GAUSSIAN_BT_0_5);

  /* amplitude shaping mode of transmitter */
  ax_hw_write_register_8(0, AX_REG_MODCFGA,
                         AX_MODCFGA_TXDIFF | AX_MODCFGA_AMPLSHAPE_RAISED_COSINE);

  /* TX deviation */
  switch (mod->modulation) {
    case AX_MODULATION_PSK:     /* PSK here?????? */
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
  ax_hw_write_register_24(0, AX_REG_FSKDEV, fskdev);
  /* 0x2AB = 683. f_deviation = 666Hz... */
  debug_printf("fskdev %d = 0x%06x\n", deviation, fskdev);


  /* TX bitrate. We assume bitrate < f_xtal */
  txrate = (uint32_t)((((float)mod->bitrate * (1 << 24)) /
                       (float)config->f_xtal) + 0.5);
  ax_hw_write_register_24(0, AX_REG_TXRATE, txrate);

  debug_printf("bitrate %d = 0x%06x\n", mod->bitrate, txrate);

  /* check bitrate for asynchronous wire mode */
  if (1 && mod->bitrate >= config->f_xtal / 32) {
    debug_printf("for asynchronous wire mode, bitrate must be less than f_xtal/32\n");
  }

  /* TX power */
  pwr = (uint16_t)((mod->power * (1 << 12)) + 0.5);
  pwr = (pwr > 0xFFF) ? 0xFFF : pwr; /* max 0xFFF */
  ax_hw_write_register_16(0, AX_REG_TXPWRCOEFFB, pwr);

  debug_printf("power %f = 0x%03x\n", mod->power, pwr);
}

/**
 * 5.17 set PLL parameters
 */
void ax_set_pll_parameters(ax_config* config)
{
  uint8_t pllrngclk_div;

  /* VCO Current - 1250 uA VCO1, 250 uA VCO2 */
  ax_hw_write_register_8(0, AX_REG_PLLVCOI,
                         AX_PLLVCOI_ENABLE_MANUAL | 25);

  /* PLL Ranging Clock */
  pllrngclk_div = AX_PLLRNGCLK_DIV_2048;
  ax_hw_write_register_8(0, AX_REG_PLLRNGCLK, pllrngclk_div);
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

    ax_hw_write_register_8(0, AX_REG_XTALCAP, xtalcap);
  }

  /* Crystal Oscillator Control */
  if (config->f_xtal > 43*1000*1000) {
    xtalosc = 0x0D;             /* > 43 MHz */
  } else if (config->clock_source == AX_CLOCK_SOURCE_TCXO) {
    xtalosc = 0x04;             /* TCXO */
  } else {
    xtalosc = 0x03;             /*  */
  }
  ax_hw_write_register_8(0, AX_REG_XTALOSC, xtalosc);

  /* Crystal Oscillator Amplitude Control */
  if (config->clock_source == AX_CLOCK_SOURCE_TCXO) {
    xtalampl = 0x00;
  } else {
    xtalampl = 0x07;
  }
  ax_hw_write_register_8(0, AX_REG_XTALAMPL, xtalampl);

  /* F35 */
  if (config->f_xtal < 24800*1000) {
    f35 = 0x10;                 /* < 24.8 MHz */
    config->f_xtaldiv = 1;
  } else {
    f35 = 0x11;
    config->f_xtaldiv = 2;
  }
  ax_hw_write_register_8(0, 0xF35, f35);
}
/**
 * 5.19 set baseband parameters
 */
void ax_set_baseband_parameters()
{
  ax_hw_write_register_8(0, AX_REG_BBTUNE, 0x0F);
  /* Baseband tuning value 0xF */

  ax_hw_write_register_8(0, AX_REG_BBOFFSCAP, 0x77);
  /* Offset capacitors all ones */
}
/**
 * 5.20 set packet format parameters
 */
void ax_set_packet_parameters()
{
  ax_hw_write_register_8(0, AX_REG_PKTADDRCFG, 0x01);
  /* address at position 1 */

  ax_hw_write_register_8(0, AX_REG_PKTLENCFG, 0x80);
  /* 8 significant bits on length byte */

  ax_hw_write_register_8(0, AX_REG_PKTLENOFFSET, 0x00);
  /* zero offset on length byte */

  /* Maximum packet length */
  ax_hw_write_register_8(0, AX_REG_PKTMAXLEN, 0xC8);
  /* 0xC8 = 200 bytes */
}
/**
 * 5.21 set match parameters
 */
void ax_set_match_parameters()
{
  /* match 1, then match 0 */

  ax_hw_write_register_32(0, AX_REG_MATCH0PAT, 0xAACCAACC);
  ax_hw_write_register_16(0, AX_REG_MATCH1PAT, 0x7E7E);

  ax_hw_write_register_8(0, AX_REG_MATCH1LEN, 0x8A);
  /* Raw received bits, 11-bit pattern */

  ax_hw_write_register_8(0, AX_REG_MATCH1MAX, 0x0A);
  /* signal a match if recevied bitstream matches for 10-bits or more */
}
/**
 * 5.22 set packet controller parameters
 */
void ax_set_packet_controller_parameters()
{
  ax_hw_write_register_8(0, AX_REG_TMGTXBOOST, 0x33); /* 38us pll boost time */
  ax_hw_write_register_8(0, AX_REG_TMGTXSETTLE, 0x14); /* 20us tx pll settle time  */
  ax_hw_write_register_8(0, AX_REG_TMGRXBOOST, 0x33);  /* 38us rx pll boost time */
  ax_hw_write_register_8(0, AX_REG_TMGRXSETTLE, 0x14); /* 20us rx pll settle time */
  ax_hw_write_register_8(0, AX_REG_TMGRXOFFSACQ, 0x00); /* 0us bb dc offset aquis tim */
  ax_hw_write_register_8(0, AX_REG_TMGRXCOARSEAGC, 0x73); /* 152 us rx agc coarse  */
  ax_hw_write_register_8(0, AX_REG_TMGRXRSSI, 0x03);      /* 3us rssi setting time */
  ax_hw_write_register_8(0, AX_REG_TMGRXPREAMBLE2, 0x17); /* 23 bit preamble timeout */
  ax_hw_write_register_8(0, AX_REG_RSSIABSTHR, 0xDD);     /* rssi threashold = 221 */
  ax_hw_write_register_8(0, AX_REG_BGNDRSSITHR, 0x00);

  /* max chunk size = 240 bytes */
  ax_hw_write_register_8(0, AX_REG_PKTCHUNKSIZE,
                         AX_PKT_MAXIMUM_CHUNK_SIZE_240_BYTES);

  /* accept multiple chunks */
  ax_hw_write_register_8(0, AX_REG_PKTACCEPTFLAGS,
                         AX_PKT_ACCEPT_MULTIPLE_CHUNKS);
}



/**
 * Wait for oscillator running and stable
 */
void ax_wait_for_oscillator(void)
{
  int i = 0;
  while (!(ax_hw_read_register_8(0, AX_REG_XTALSTATUS) & 1)) {
    i++;
  }

  debug_printf("osc stable in %d cycles\n", i);
}



void ax5043_set_registers(ax_config* config, ax_modulation* mod)
{
  /* ax_hw_write_register_8(0, AX_REG_MODULATION, 0x08); */
  /* ax_hw_write_register_8(0, AX_REG_ENCODING, 0x00); */
  /* ax_hw_write_register_8(0, AX_REG_FRAMING, 0x24); */
  /* ax_hw_write_register_8(0, AX_REG_FEC, 0x13); */
  //
  ax_set_modulation_parameters(config, mod);

  ax_hw_write_register_8(0, AX_REG_PINFUNCSYSCLK, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCDCLK, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCDATA, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCANTSEL, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCPWRAMP, 0x07);
  ax_hw_write_register_8(0, AX_REG_WAKEUPXOEARLY, 0x01);

  /* ax_hw_write_register_16(0, AX_REG_IFFREQ, 0x00C8); */
  /* ax_hw_write_register_8(0, AX_REG_DECIMATION, 0x44); */
  /* ax_hw_write_register_24(0, AX_REG_RXDATARATE, 0x003C2E); */
  /* ax_hw_write_register_24(0, AX_REG_MAXDROFFSET, 0x0); */
  /* ax_hw_write_register_24(0, AX_REG_MAXRFOFFSET, 0x80037B); */
  /* ax_hw_write_register_16(0, AX_REG_FSKDMAX, 0x00A6); */
  /* ax_hw_write_register_16(0, AX_REG_FSKDMIN, 0xFF5A); */
  /* ax_hw_write_register_8(0, AX_REG_AMPLFILTER, 0x00); */
  //
  ax_set_rx_parameters(config, mod);


  ax_hw_write_register_8(0, AX_REG_RXPARAMSETS, 0xF4);
  /* 0, 1, 3, 3 */

  /* RX 0 */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_AGCGAIN, 0xD7);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_TIMEGAIN, 0xF8);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_DRGAIN, 0xF2);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAINC, 0x0B);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_FREQUENCYGAIND, 0x0B);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(0, AX_REG_RX_PARAMETER0 + AX_RX_FREQDEV, 0x0000);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER0 + AX_RX_BBOFFSRES, 0x00);
  /* RX 1 */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_AGCGAIN, 0xD7);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_AGCAHYST, 0x00);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_AGCMINMAX, 0x00);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_TIMEGAIN, 0xF6);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_DRGAIN, 0xF1);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAINC, 0x0B);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQUENCYGAIND, 0x0B);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQDEV, 0x0043);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER1 + AX_RX_BBOFFSRES, 0x00);
  /* RX 3 */
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCGAIN, 0xFF);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCTARGET, 0x84);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCAHYST, 0x00);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AGCMINMAX, 0x00);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_TIMEGAIN, 0xF5);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_DRGAIN, 0xF0);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_PHASEGAIN, 0xC3);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINA, 0x0F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINB, 0x1F);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAINC, 0x0D);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FREQUENCYGAIND, 0x0D);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_AMPLITUDEGAIN, 0x06);
  ax_hw_write_register_16(0, AX_REG_RX_PARAMETER1 + AX_RX_FREQDEV, 0x0043);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_FOURFSK, 0x16);
  ax_hw_write_register_8(0, AX_REG_RX_PARAMETER3 + AX_RX_BBOFFSRES, 0x00);

  /* ax_hw_write_register_8(0, AX_REG_MODCFGF, 0x03); */
  /* ax_hw_write_register_24(0, AX_REG_FSKDEV, 0x0002AB); */
  /* ax_hw_write_register_8(0, AX_REG_MODCFGA, 0x05); */
  /* ax_hw_write_register_24(0, AX_REG_TXRATE, 0x000802); */
  /* ax_hw_write_register_16(0, AX_REG_TXPWRCOEFFB, 0x0FFF); */
  //
  ax_set_tx_parameters(config, mod);

  /* ax_hw_write_register_8(0, AX_REG_PLLVCOI, 0x99); */
  /* ax_hw_write_register_8(0, AX_REG_PLLRNGCLK, 0x03); */
  //
  ax_set_pll_parameters(config);

  /* ax_hw_write_register_8(0, AX_REG_BBTUNE, 0x0F); */
  /* ax_hw_write_register_8(0, AX_REG_BBOFFSCAP, 0x77); */
  //
  ax_set_baseband_parameters();

  /* ax_hw_write_register_8(0, AX_REG_PKTADDRCFG, 0x01); */
  /* ax_hw_write_register_8(0, AX_REG_PKTLENCFG, 0x80); */
  /* ax_hw_write_register_8(0, AX_REG_PKTLENOFFSET, 0x00); */
  /* ax_hw_write_register_8(0, AX_REG_PKTMAXLEN, 0xC8); */
  //
  ax_set_packet_parameters();

  /* ax_hw_write_register_32(0, AX_REG_MATCH0PAT, 0xAACCAACC); */
  /* ax_hw_write_register_16(0, AX_REG_MATCH1PAT, 0x7E7E); */
  /* ax_hw_write_register_8(0, AX_REG_MATCH1LEN, 0x8A); */
  /* ax_hw_write_register_8(0, AX_REG_MATCH1MAX, 0x0A); */
  //
  ax_set_match_parameters();

  /* Packet controller */
  /* ax_hw_write_register_8(0, AX_REG_TMGTXBOOST, 0x33); */
  /* ax_hw_write_register_8(0, AX_REG_TMGTXSETTLE, 0x14); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXBOOST, 0x33); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXSETTLE, 0x14); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXOFFSACQ, 0x00); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXCOARSEAGC, 0x73); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXRSSI, 0x03); */
  /* ax_hw_write_register_8(0, AX_REG_TMGRXPREAMBLE2, 0x17); */
  /* ax_hw_write_register_8(0, AX_REG_RSSIABSTHR, 0xDD); */
  /* ax_hw_write_register_8(0, AX_REG_BGNDRSSITHR, 0x00); */
  /* ax_hw_write_register_8(0, AX_REG_PKTCHUNKSIZE, 0x0D); */
  /* ax_hw_write_register_8(0, AX_REG_PKTACCEPTFLAGS, 0x20); */
  //
  ax_set_packet_controller_parameters();

  ax_hw_write_register_8(0, AX_REG_DACCONFIG, 0x00);
  ax_hw_write_register_8(0, AX_REG_REF, 0x03);
  /* ax_hw_write_register_8(0, AX_REG_XTALOSC, 0x04); */
  /* ax_hw_write_register_8(0, AX_REG_XTALAMPL, 0x00); */

  ax_hw_write_register_8(0, 0xF1C, 0x07); /* const */
  ax_hw_write_register_8(0, 0xF21, 0x68); /* !! */
  ax_hw_write_register_8(0, 0xF22, 0xFF); /* !! */
  ax_hw_write_register_8(0, 0xF23, 0x84); /* !! */
  ax_hw_write_register_8(0, 0xF26, 0x98); /* !! */
  /* ax_hw_write_register_8(0, 0xF34, 0x28); /\* done *\/ */
  /* ax_hw_write_register_8(0, 0xF35, 0x10); /\* done *\/ */
  ax_hw_write_register_8(0, 0xF44, 0x25); /* ?? */
}


void ax5043_set_registers_tx(ax_config* config)
{
  ax_set_synthesiser_parameters(&synth_operation,
                                &config->synthesiser.A,
                                config->synthesiser.vco_type);

  ax_hw_write_register_8(0, 0xF00, 0x0F); /* const */
  ax_hw_write_register_8(0, 0xF18, 0x06); /* ?? */
}


void ax5043_set_registers_rx(ax_config* config)
{
  ax_set_synthesiser_parameters(&synth_operation,
                                &config->synthesiser.A,
                                config->synthesiser.vco_type);

  ax_hw_write_register_8(0, 0xF00, 0x0F); /* const */
  ax_hw_write_register_8(0, 0xF18, 0x02); /* ?? */
}


/**
 * MAJOR FUNCTIONS ------------------------------------------
 */

/**
 * Performs a ranging operation
 *
 * updates values in synth structure
 */
void ax_do_vco_ranging(uint16_t pllranging,
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
  ax_set_synthesiser_parameters(&synth_ranging, synth, vco_type);


  /* Set RNGSTART bit (PLLRANGINGA,B) */
  ax_hw_write_register_8(0, pllranging,
                         synth->vco_range | AX_PLLRANGING_RNG_START);

  /* Wait for RNGSTART bit to clear */
  do {
    r = ax_hw_read_register_8(0, pllranging);
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

  /* Set PWRMODE to STANDBY */
  ax_set_pwrmode(config, AX_PWRMODE_STANDBY);

  /* Set FREQA,B registers to correct value */
  ax_set_synthesiser_frequencies(config);

  /* Manual VCO current, 27 = 1350uA VCO1, 270uA VCO2 */
  ax_hw_write_register_8(0, AX_REG_PLLVCOI,
                         AX_PLLVCOI_ENABLE_MANUAL | 27);

  /* Wait for oscillator to be stable */
  ax_wait_for_oscillator();

  /* do ranging */
  ax_do_vco_ranging(AX_REG_PLLRANGINGA,
                    &config->synthesiser.A, config->synthesiser.vco_type);
  ax_do_vco_ranging(AX_REG_PLLRANGINGB,
                    &config->synthesiser.B, config->synthesiser.vco_type);


  /* Set PWRMODE to POWERDOWN */
  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  /* Disable TCXO if used */
}

/**
 * PUBLIC FUNCTIONS ------------------------------------------
 */

void ax_tx_on(ax_config* config, ax_modulation* mod)
{
  debug_printf("going for transmit...\n");

  ax5043_set_registers(config, mod);
  ax5043_set_registers_tx(config);    /* set tx registers??? */

  /* Place chip in FULLTX mode */
  ax_set_pwrmode(config, AX_PWRMODE_FULLTX);

  /* Enable TCXO if used */

  /* Wait for oscillator to start running  */
  ax_wait_for_oscillator();

  /* Set PWRMODE to POWERDOWN */
  //ax_set_pwrmode(AX_PWRMODE_DEEPSLEEP);

  /* Disable TCXO if used */
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
  while (!(ax_hw_read_register_8(0, AX_REG_POWSTAT) & AX_POWSTAT_SVMODEM));

  /* Write preamble and packet to the FIFO */
  ax_fifo_tx_data(packet, length);

  /* Commit FIFO contents */
  ax_fifo_commit();

  debug_printf("packet committed to FIFO!\n");
}

/**
 * First attempt at receiver, don't care about power
 */
void ax_rx_on(ax_config* config, ax_modulation* mod)
{
  ax_rx_chunk rx_chunk;

  /* TODO set defaults in modulation structure  */

  /* Meta-data can be automatically added to FIFO, see PKTSTOREFLAGS */

  /* Place chip in FULLRX mode */
  ax_set_pwrmode(config, AX_PWRMODE_FULLRX);

  ax5043_set_registers_rx(config);    /* set rx registers??? */

  /* Enable TCXO if used */

  /* Clear FIFO */
  ax_fifo_clear();

  while (1) {
    /* Check if FIFO is not empty */
    if (ax_fifo_rx_data(0, &rx_chunk)) {

      /* Got something */
      if (rx_chunk.chunk_t == AX_FIFO_CHUNK_DATA) {

        for (int i = 0; i < rx_chunk.chunk.data.length; i++) {
          debug_printf("data %d: 0x%02x %c\n", i,
                       rx_chunk.chunk.data.data[i],
                       rx_chunk.chunk.data.data[i]);
        }

      } else {
        debug_printf("some other chunk type 0x%02x\n", rx_chunk.chunk_t);
      }
    }
  }
}

/**
 * Waits for any ongoing operations to complete, and then shuts down the radio
 */
void ax_off(ax_config* config)
{
  /* Wait for any ongoing operations to complete by polling RADIOSTATE */
  while ((ax_hw_read_register_8(0, AX_REG_RADIOSTATE) & 0xF) > 1) {
    /* Not IDLE or POWERDOWN */
  }

  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  debug_printf("ax_off complete!\n");
}

/**
 * First attempt at Initialisation
 */
void ax_init(ax_config* config)
{
  //config.max_delta_carrier = 870; // 2ppm TODO ppm calculations

  /* Reset the chip */

  /* Set SEL high for at least 1us, then low */

  /* Wait for MISO to go high */

  /* Set RST bit (PWRMODE) */
  ax_hw_write_register_8(0, AX_REG_PWRMODE, AX_PWRMODE_RST);

  /* AX is now in reset */

  /* Set the PWRMODE register to POWERDOWN, also clears RST bit */
  ax_set_pwrmode(config, AX_PWRMODE_POWERDOWN);

  /* Program parameters.. (these could initially come from windows software, or be calculated) */
  ax_set_xtal_parameters(config);

  /* Perform auto-ranging for both VCOs */
  ax_vco_ranging(config);
}

/**
 * Read silicon revision register
 */
uint8_t ax_silicon_revision(int channel)
{
  return ax_hw_read_register_8(channel, AX_REG_SILICONREVISION);
}
/**
 * Read scratch register
 */
uint8_t ax_scratch(int channel)
{
  return ax_hw_read_register_8(channel, AX_REG_SCRATCH);
}
