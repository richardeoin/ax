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

#include "ax_hw.h"
#include "ax_reg.h"

#define TCXO 1



void ax_transmit(void)
{
  /* Place chip in FULLTX mode */

  /* Enable TCXO if used */

  /* Ensure the SVMODEM bit (POWSTAT) is set high (See 3.1.1) */

  /* Write preamble and packet to the FIFO */

  /* Wait for oscillator to start running  */

  /* Commit FIFO contents */

  /* Wait for transmit to complete by polling RADIOSTATE */

  /* Disable TCXO if used */
}

void ax_vco_ranging()
{
  /**
   * re-ranging is required for > 5MHz in 868/915 or > 2.5MHz in 433
   */

  /* Set FREQA,B registers to correct value */

  /* Possibly set VCORA,B, good default is 8 */

  /* Set PWRMODE to STANDBY */

  /* Enable TCXO if used */

  /* Wait for oscillator to start running */

  /* Set RNGSTART bit (PLLRANGINGA,B) */

  /* Wait for RNGSTART bit to clear */

  /* Check RNGERR bit */

  /* Set PWRMODE to POWERDOWN */

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
  /* Reset the chip */

  /* Set SEL high for at least 1us, then low */

  /* Wait for MISO to go high */

  /* Set and clear RST bit (PWRMODE) */

  /* Set the PWRMODE register to POWERDOWN */

  /* Program parameters.. (these could initially coe from windows software, or be calculated) */

  /* Perform auto-ranging for VCO */

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
