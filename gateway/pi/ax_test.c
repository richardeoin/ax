/*
 * Test code for ax on raspberry pi
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

#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Wiring pi */
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "ax/ax.h"
#include "ax/ax_hw.h"
#include "ax/ax_modes.h"
#include "ax/ax_reg_values.h"


#define SPI_CHANNEL	0           /* channel */
#define SPI_SPEED	5000000     /* 5MHz */


void wiringpi_spi_transfer(unsigned char* data, uint8_t length)
{
  wiringPiSPIDataRW(SPI_CHANNEL, data, length);
}
void rx_callback(unsigned char* data, uint8_t length)
{
  printf("Rx: %s\n", (char*)data);
}


int main()
{
  if (wiringPiSPISetup(0, SPI_SPEED) < 0) {
    fprintf(stderr, "Failed to open SPI port.  Try loading spi library with 'gpio load spi'");
  }


  uint8_t pkt[0x100];

  ax_config config;
  memset(&config, 0, sizeof(ax_config));

  config.clock_source = AX_CLOCK_SOURCE_TCXO;
  config.f_xtal = 16369000;

  config.synthesiser.A.frequency = 434600000;
  config.synthesiser.B.frequency = 434600000;

  config.spi_transfer = wiringpi_spi_transfer;

  config.pkt_store_flags = AX_PKT_STORE_RSSI |
    AX_PKT_STORE_RF_OFFSET;

  ax_init(&config);
  //ax_tx_on(&config, &fsk1_modulation);
  ax_rx_on(&config, &fsk1_modulation);

  while (1) {
    //sleep(1);

    strcpy((char*)pkt, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    ax_tx_packet(&config, pkt, 40);
  }

  return 0;
}
