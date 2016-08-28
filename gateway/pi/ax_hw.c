/*
 * Functions for accessing ax hardware using wiring pi
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

#include <stdio.h>

/* Wiring pi */
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "ax_hw.h"
#include "ax.h"

#define SPI_SPEED	500000      /* 500kHz */

/* Current status */
uint16_t status = 0;

/**
 * Reads register, and fully updates status. 8 bit
 *
 * Returns result
 */
uint8_t ax_hw_read_register_long_8(int channel, uint16_t reg)
{
  unsigned char data[3];

  data[0] = ((reg >> 8) | 0x70);
  data[1] = (reg & 0xFF);
  data[2] = 0xFF;
  wiringPiSPIDataRW(channel, data, 3);

  status = ((uint16_t)data[0] << 8) & data[1];

  return (uint8_t)data[2];
}
/**
 * Writes register, and fully updates status. 8 bit
 *
 * Returns status
 */
uint16_t ax_hw_write_register_long_8(int channel, uint16_t reg, uint8_t value)
{
  unsigned char data[3];

  data[0] = ((reg >> 8) | 0xF0);
  data[1] = (reg & 0xFF);
  data[2] = value;
  wiringPiSPIDataRW(channel, data, 3);

  status = ((uint16_t)data[0] << 8) & data[1];

  return status;
}
/**
 * Reads register, using long or short access as required. 8 bit
 *
 * Returns result
 */
uint8_t ax_hw_read_register_8(int channel, uint16_t reg)
{
  if (reg > 0x70) {             /* long access */
    return ax_hw_read_register_long_8(channel, reg);

  } else {                      /* short access */
    unsigned char data[2];

    data[0] = (reg & 0x7F);
    data[1] = 0xFF;
    wiringPiSPIDataRW(channel, data, 2);

    status &= 0xFF;
    status |= ((uint16_t)data[0] << 8);

    return (uint8_t)data[1];
  }
}
/**
 * Write register, using long or short access as required. 8 bit
 *
 * Returns status
 */
uint16_t ax_hw_write_register_8(int channel, uint16_t reg, uint8_t value)
{
  if (reg > 0x70) {             /* long access */
    return ax_hw_write_register_long_8(channel, reg, value);

  } else {                      /* short access */
    unsigned char data[2];

    data[0] = ((reg & 0x7F) | 0x80);
    data[1] = value;
    wiringPiSPIDataRW(channel, data, 2);

    status &= 0xFF;
    status |= ((uint16_t)data[0] << 8);

    return status;
  }
}

/**
 * Returns the status from the last transaction
 */
uint16_t ax_hw_status(void)
{
  return status;
}
/**
 * Polls the hardware for the latest status, and returns it.
 */
uint16_t ax_hw_poll_status(void)
{
  return 0;
}

/**
 * Attempts to initialise the ax hardware
 *
 * Returns 1 on success, 0 otherwise
 */
int ax_hw_init(int channel)
{
  if (wiringPiSPISetup(channel, SPI_SPEED) < 0) {
    fprintf(stderr, "Failed to open SPI port.  Try loading spi library with 'gpio load spi'");
    return AX_INIT_PORT_FAILED;
  }

  /* Scratch */
  uint8_t scratch = ax_scratch(channel);
  printf("Scratch 0x%X\n", scratch);

  if (scratch != AX_SCRATCH) {
    printf("Bad scratch value.\n");

    return AX_INIT_BAD_SCRATCH;
  }

  /* Revision */
  uint8_t silicon_revision = ax_silicon_revision(channel);
  printf("Silcon Revision 0x%X\n", silicon_revision);

  if (silicon_revision != AX_SILICONREVISION) {
    printf("Bad Silcon Revision value.\n");

    return AX_INIT_BAD_REVISION;
  }

  return AX_INIT_OK;
}