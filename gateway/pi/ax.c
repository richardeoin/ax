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

#include "ax.h"
#include "ax_hw.h"
#include "ax_reg.h"
#include "ax_reg_values.h"
#include "ax_fifo.h"

#define TCXO 1

#include <stdio.h>
#define debug_printf printf

void ax_set_tx_power(float power);

void ax5043_set_registers(void)
{
  ax_hw_write_register_8(0, AX_REG_MODULATION, 0x08);
  ax_hw_write_register_8(0, AX_REG_ENCODING, 0x00);
  ax_hw_write_register_8(0, AX_REG_FRAMING, 0x24);
  ax_hw_write_register_8(0, AX_REG_FEC, 0x13);
  ax_hw_write_register_8(0, AX_REG_PINFUNCSYSCLK, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCDCLK, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCDATA, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCANTSEL, 0x01);
  ax_hw_write_register_8(0, AX_REG_PINFUNCPWRAMP, 0x07);
  ax_hw_write_register_8(0, AX_REG_WAKEUPXOEARLY, 0x01);

  ax_hw_write_register_16(0, AX_REG_IFFREQ, 0x00C8);

  ax_hw_write_register_8(0, AX_REG_DECIMATION, 0x44);

  ax_hw_write_register_24(0, AX_REG_RXDATARATE, 0x003C2E);

  ax_hw_write_register_24(0, AX_REG_MAXDROFFSET, 0x0);
  ax_hw_write_register_24(0, AX_REG_MAXRFOFFSET, 0x80037B);

  ax_hw_write_register_16(0, AX_REG_FSKDMAX, 0x00A6);
  ax_hw_write_register_16(0, AX_REG_FSKDMIN, 0xFF5A);

  ax_hw_write_register_8(0, AX_REG_AMPLFILTER, 0x00);
  ax_hw_write_register_8(0, AX_REG_RXPARAMSETS, 0xF4);

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

  ax_hw_write_register_8(0, AX_REG_MODCFGF, 0x03);

  ax_hw_write_register_24(0, AX_REG_FSKDEV, 0x0002AB);

  ax_hw_write_register_8(0, AX_REG_MODCFGA, 0x05);


  ax_hw_write_register_24(0, AX_REG_TXRATE, 0x000802);

  //ax_hw_write_register_16(0, AX_REG_TXPWRCOEFFB, 0x0FFF);
  ax_set_tx_power(1.0);

  ax_hw_write_register_8(0, AX_REG_PLLVCOI, 0x99);
  /* PLL Ranging clock = f_XTAL / 2^11 */
  ax_hw_write_register_8(0, AX_REG_PLLRNGCLK, 0x03);
  ax_hw_write_register_8(0, AX_REG_BBTUNE, 0x0F);
  ax_hw_write_register_8(0, AX_REG_BBOFFSCAP, 0x77);
  ax_hw_write_register_8(0, AX_REG_PKTADDRCFG, 0x01);
  ax_hw_write_register_8(0, AX_REG_PKTLENCFG, 0x80);
  ax_hw_write_register_8(0, AX_REG_PKTLENOFFSET, 0x00);
  ax_hw_write_register_8(0, AX_REG_PKTMAXLEN, 0xC8);

  ax_hw_write_register_32(0, AX_REG_MATCH0PAT, 0xAACCAACC);

  ax_hw_write_register_16(0, AX_REG_MATCH1PAT, 0x7E7E);

  ax_hw_write_register_8(0, AX_REG_MATCH1LEN, 0x8A);
  ax_hw_write_register_8(0, AX_REG_MATCH1MAX, 0x0A);
  ax_hw_write_register_8(0, AX_REG_TMGTXBOOST, 0x33);
  ax_hw_write_register_8(0, AX_REG_TMGTXSETTLE, 0x14);
  ax_hw_write_register_8(0, AX_REG_TMGRXBOOST, 0x33);
  ax_hw_write_register_8(0, AX_REG_TMGRXSETTLE, 0x14);
  ax_hw_write_register_8(0, AX_REG_TMGRXOFFSACQ, 0x00);
  ax_hw_write_register_8(0, AX_REG_TMGRXCOARSEAGC, 0x73);
  ax_hw_write_register_8(0, AX_REG_TMGRXRSSI, 0x03);
  ax_hw_write_register_8(0, AX_REG_TMGRXPREAMBLE + 1, 0x17);
  ax_hw_write_register_8(0, AX_REG_RSSIABSTHR, 0xDD);
  ax_hw_write_register_8(0, AX_REG_BGNDRSSITHR, 0x00);
  ax_hw_write_register_8(0, AX_REG_PKTCHUNKSIZE, 0x0D);
  ax_hw_write_register_8(0, AX_REG_PKTACCEPTFLAGS, 0x20);

  ax_hw_write_register_8(0, AX_REG_DACCONFIG, 0x00);
  ax_hw_write_register_8(0, AX_REG_REF, 0x03);
  ax_hw_write_register_8(0, AX_REG_XTALOSC, 0x04);
  ax_hw_write_register_8(0, AX_REG_XTALAMPL, 0x00);

  ax_hw_write_register_8(0, 0xF1C, 0x07);
  ax_hw_write_register_8(0, 0xF21, 0x68);
  ax_hw_write_register_8(0, 0xF22, 0xFF);
  ax_hw_write_register_8(0, 0xF23, 0x84);
  ax_hw_write_register_8(0, 0xF26, 0x98);
  ax_hw_write_register_8(0, 0xF34, 0x28);
  ax_hw_write_register_8(0, 0xF35, 0x10);
  ax_hw_write_register_8(0, 0xF44, 0x25);
}


void ax5043_set_registers_tx(void)
{
  /* Bypass external filter, Internall loop filter BW=500kHz */
  ax_hw_write_register_8(0, AX_REG_PLLLOOP, 0x0B);
  /* Charge Pump I = 136uA */
  ax_hw_write_register_8(0, AX_REG_PLLCPI, 0x10);
  /* f_PD = f_XTAL, divide RF by 2, internal VCO2 */
  ax_hw_write_register_8(0, AX_REG_PLLVCODIV, 0x24);
  /* No XTAL load capacitance */
  ax_hw_write_register_8(0, AX_REG_XTALCAP, 0x00);
  ax_hw_write_register_8(0, 0xF00, 0x0F);
  ax_hw_write_register_8(0, 0xF18, 0x06);
}


void ax5043_set_registers_rx(void)
{
  ax_hw_write_register_8(0, AX_REG_PLLLOOP, 0x0B);
  ax_hw_write_register_8(0, AX_REG_PLLCPI, 0x10);
  ax_hw_write_register_8(0, AX_REG_PLLVCODIV, 0x24);
  ax_hw_write_register_8(0, AX_REG_XTALCAP, 0x00);
  ax_hw_write_register_8(0, 0xF00, 0x0F);
  ax_hw_write_register_8(0, 0xF18, 0x02);
}


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
 * one DATA command
 */
void ax_fifo_tx_data(uint8_t* data, uint8_t length)
{
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
 * Commits data written to the fifo
 */
void ax_fifo_commit(void)
{
  ax_hw_write_register_8(0, AX_REG_FIFOSTAT, AX_FIFOCMD_COMMIT);
}

/**
 * UTILITY FUNCTIONS ----------------------------------------
 */

void ax_set_pwrmode(uint8_t pwrmode)
{
  ax_hw_write_register_8(0, AX_REG_PWRMODE, pwrmode); /* TODO R-m-w */
}

/**
 * Sets a PLL to a given frequency.
 */
void ax_set_frequency(ax_config* config, enum ax_pll pll, uint32_t frequency)
{
  uint32_t freq;
  uint16_t reg_freq;

  switch (pll) {
    case AX_PLL_A: reg_freq = AX_REG_FREQA; break;
    case AX_PLL_B: reg_freq = AX_REG_FREQB; break;
    default: return;
  }

  /* we choose to always set the LSB to avoid spectral tones */
  freq = (uint32_t)(((double)frequency * (1 << 23)) / (float)config->f_xtal);
  freq = (freq << 1) | 1;

  debug_printf("freq %d = 0x%08x\n", frequency, freq);

  ax_hw_write_register_32(0, reg_freq, freq);
}
/**
 * Set modulation
 */
void ax_set_modulation(ax_config* config, uint32_t bitrate)
{
  uint32_t txrate;

  /* amplitude shaping mode of transmitter */

  /* bitrate. We assume bitrate < f_xtal */
  txrate = (uint32_t)((((float)bitrate * (1<<24)) / (float)config->f_xtal) + 0.5);
  ax_hw_write_register_24(0, AX_REG_TXRATE, txrate);

  /* check bitrate for asynchronous wire mode */
  if (1 && bitrate >= config->f_xtal / 32) {
    debug_printf("for asynchronous wire mode, bitrate must be less than f_xtal/32\n");
  }
}


/**
 * Sets a given transmit power, as a fraction of maximum
 *
 * Pre-distortion is possible in hardware, but not supported here.
 */
void ax_set_tx_power(float power)
{
  uint16_t pwr;

  pwr = (uint16_t)((power * (1 << 12)) + 0.5);
  pwr = (pwr > 0xFFF) ? 0xFFF : pwr; /* max 0xFFF */

  debug_printf("power %f = 0x%03x\n", power, pwr);

  ax_hw_write_register_16(0, AX_REG_TXPWRCOEFFB, pwr);
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


/**
 * MAJOR FUNCTIONS ------------------------------------------
 */

void ax_transmit(void)
{
  debug_printf("going for transmit...\n");

  /* Place chip in FULLTX mode */
  ax_set_pwrmode(AX_PWRMODE_FULLTX);

  ax5043_set_registers_tx();    /* set tx registers??? */

  /* Enable TCXO if used */

  /* Ensure the SVMODEM bit (POWSTAT) is set high (See 3.1.1) */
  while (!(ax_hw_read_register_8(0, AX_REG_POWSTAT) & AX_POWSTAT_SVMODEM));

  /* Write preamble and packet to the FIFO */
  ax_fifo_tx_data((uint8_t*)"hello", 5);

  /* Wait for oscillator to start running  */
  ax_wait_for_oscillator();

  /* Commit FIFO contents */
  ax_fifo_commit();

  /* Wait for transmit to complete by polling RADIOSTATE */
  while ((ax_hw_read_register_8(0, AX_REG_RADIOSTATE) & 0xF) != AX_RADIOSTATE_IDLE);

  debug_printf("transmit complete!\n");

  /* Set PWRMODE to POWERDOWN */
  //ax_set_pwrmode(AX_PWRMODE_DEEPSLEEP);

  /* Disable TCXO if used */
}

void ax_vco_ranging(ax_config* config, enum ax_pll pll)
{
  uint8_t r;

  /**
   * re-ranging is required for > 5MHz in 868/915 or > 2.5MHz in 433
   */

  debug_printf("starting vco ranging...\n");

  /* Set PWRMODE to STANDBY */
  ax_set_pwrmode(AX_PWRMODE_STANDBY);


  /* Set FREQA,B registers to correct value */
  ax_set_frequency(config, pll, 434600000);

  r = ax_hw_read_register_8(0, AX_REG_FREQA);
  debug_printf("FREQA r = 0x%02x\n", r);
  r = ax_hw_read_register_8(0, AX_REG_FREQA+1);
  debug_printf("FREQA r = 0x%02x\n", r);
  r = ax_hw_read_register_8(0, AX_REG_FREQA+2);
  debug_printf("FREQA r = 0x%02x\n", r);
  r = ax_hw_read_register_8(0, AX_REG_FREQA+3);
  debug_printf("FREQA r = 0x%02x\n", r);

  /* Set default 100kHz loop BW for ranging */
  /* Manual VCO current, 27 = 1350uA VCO1, 270uA VCO2 */
  ax_hw_write_register_8(0, AX_REG_PLLVCOI, 0x9B);
  /* Internal loop filter, BW=100kHz, bypass external filter pin */
  ax_hw_write_register_8(0, AX_REG_PLLLOOP, 0x09);
  /* Charge Pump I = 68uA */
  ax_hw_write_register_8(0, AX_REG_PLLCPI, 0x08);


  /* Possibly set VCORA,B, good default is 8 */
  uint8_t vcor = 8;


  /* Enable TCXO if used */

  /* Wait for oscillator to be stable */
  ax_wait_for_oscillator();

  uint16_t pllranging;
  switch (pll) {
    case AX_PLL_A: pllranging = AX_REG_PLLRANGINGA; break;
    case AX_PLL_B: pllranging = AX_REG_PLLRANGINGB; break;
    default: return;
  }

  /* Set RNGSTART bit (PLLRANGINGA,B) */
  ax_hw_write_register_8(0, pllranging,
                         vcor | AX_PLLRANGING_RNG_START);

  /* Wait for RNGSTART bit to clear */
  do {
    r = ax_hw_read_register_8(0, pllranging);
  } while (r & AX_PLLRANGING_RNG_START);

  /* Check RNGERR bit */
  if (r & AX_PLLRANGING_RNGERR) {
    /* ranging error */
    debug_printf("Ranging error!\n");
    debug_printf("r = 0x%02x\n", r);
    return;
  }

  debug_printf("Ranging done\n");
  debug_printf("r = 0x%02x\n", r);

  /* Set PWRMODE to POWERDOWN */
  //ax_set_pwrmode(AX_PWRMODE_DEEPSLEEP);

  /* Disable TCXO if used */
}

/**
 * First attempt at receiver, don't care about power
 */
void ax_receive()
{
  /* Meta-data can be automatically added to FIFO, see PKTSTOREFLAGS */
}

/**
 * First attempt at Initialisation
 */
void ax_init()
{
  ax_config config;
  config.f_xtal = 16369000;

  /* Reset the chip */

  /* Set SEL high for at least 1us, then low */

  /* Wait for MISO to go high */

  /* Set RST bit (PWRMODE) */
  ax_hw_write_register_8(0, AX_REG_PWRMODE, AX_PWRMODE_RST);

  /* AX is now in reset */

  /* Set the PWRMODE register to POWERDOWN, also clears RST bit */
  ax_set_pwrmode(AX_PWRMODE_POWERDOWN);

  /* Program parameters.. (these could initially come from windows software, or be calculated) */
  ax5043_set_registers();
  ax5043_set_registers_tx();

  /* Perform auto-ranging for VCO */
  ax_vco_ranging(&config, AX_PLL_A);
  ax_vco_ranging(&config, AX_PLL_B);
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
