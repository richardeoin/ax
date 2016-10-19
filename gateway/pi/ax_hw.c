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
#include <string.h>

#include <stdio.h>

/* Wiring pi */
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "ax_hw.h"
#include "ax.h"
#include "ax_reg.h"
#include "ax_fifo.h"

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
 * Reads register, and fully updates status. Up to 4 bytes
 *
 * Returns status
 */
uint16_t ax_hw_read_register_long_bytes(int channel, uint16_t reg,
                                        uint8_t* ptr, uint8_t bytes)
{
  unsigned char data[6];

  if (bytes > 4) return 0;        /* Up to 4 bytes! */

  data[0] = ((reg >> 8) | 0x70);
  data[1] = (reg & 0xFF);
  memset(data+2, 0xFF, bytes);
  wiringPiSPIDataRW(channel, data, 2+bytes);

  status = ((uint16_t)data[0] << 8) & data[1];

  memcpy(ptr, data+2, bytes);

  return status;
}
/**
 * Reads register, using long or short access as required. Up to 4 bytes
 *
 * Returns status
 */
uint16_t ax_hw_read_register_bytes(int channel, uint16_t reg,
                                   uint8_t* ptr, uint8_t bytes)
{
  if (reg > 0x70) {             /* long access */
    return ax_hw_read_register_long_bytes(channel, reg, ptr, bytes);

  } else {                      /* short access */
    unsigned char data[5];

    data[0] = (reg & 0x7F);
    memset(data+1, 0xFF, bytes);
    wiringPiSPIDataRW(channel, data, 1+bytes);

    status &= 0xFF;
    status |= ((uint16_t)data[0] << 8);

    memcpy(ptr, data+1, bytes);

    return status;
  }
}


/**
 * MULTIPLE BYTES ----------------------------------------
 */

/**
 * weak combinations
 */
uint16_t ax_hw_write_register_16(int channel, uint16_t reg, uint16_t value)
{
  ax_hw_write_register_8(channel,        reg,   (value >> 8)); /* MSB first */
  return ax_hw_write_register_8(channel, reg+1, (value >> 0));
}
uint16_t ax_hw_write_register_24(int channel, uint16_t reg, uint32_t value)
{
  ax_hw_write_register_8(channel,        reg,   (value >> 16)); /* MSB first */
  ax_hw_write_register_8(channel,        reg+1, (value >> 8));
  return ax_hw_write_register_8(channel, reg+2, (value >> 0));
}
uint16_t ax_hw_write_register_32(int channel, uint16_t reg, uint32_t value)
{
  ax_hw_write_register_8(channel,        reg,   (value >> 24)); /* MSB first */
  ax_hw_write_register_8(channel,        reg+1, (value >> 16));
  ax_hw_write_register_8(channel,        reg+2, (value >> 8));
  return ax_hw_write_register_8(channel, reg+3, (value >> 0));
}
uint16_t ax_hw_read_register_16(int channel, uint16_t reg)
{
  uint8_t ptr[2];
  ax_hw_read_register_bytes(channel, reg, ptr, 2);

  return ((ptr[0] << 8) | (ptr[1]));
}
uint32_t ax_hw_read_register_24(int channel, uint16_t reg)
{
  uint8_t ptr[3];
  ax_hw_read_register_bytes(channel, reg, ptr, 3);

  return ((ptr[0] << 16) | (ptr[1] << 8) | (ptr[2]));
}
uint32_t ax_hw_read_register_32(int channel, uint16_t reg)
{
  uint8_t ptr[4];
  ax_hw_read_register_bytes(channel, reg, ptr, 4);

  return ((ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]));
}


/**
 * FIFO ------------------------------------------------
 */

/**
 * Writes buffer to fifo. First byte of buffer is discarded.
 *
 * Returns status
 */
uint16_t ax_hw_write_fifo(int channel, uint8_t* buffer, uint16_t length)
{
  uint8_t data[0x100];

  /* write (short access) */
  data[0] = ((AX_REG_FIFODATA & 0x7F) | 0x80);
  memcpy(data+1, buffer, length);

  wiringPiSPIDataRW(channel, data, length+1);

  status &= 0xFF;
  status |= ((uint16_t)data[0] << 8);

  return status;
}
/**
 * Reads buffer from fifo. First byte of returned buffer is top byte of status
 *
 * Returns status
 */
uint16_t ax_hw_read_fifo(int channel, uint8_t* buffer, uint16_t length)
{
  /* read (short access) */
  buffer[0] = (AX_REG_FIFODATA & 0x7F);

  wiringPiSPIDataRW(channel, buffer, length);

  status &= 0xFF;
  status |= ((uint16_t)buffer[0] << 8);

  return status;
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
