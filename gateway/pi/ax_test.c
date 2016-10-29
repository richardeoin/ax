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


uint8_t ax25_frame[0x100];
#define AX25_CONTROL_WORD   0x03 /* Use Unnumbered Information (UI) frames */
#define AX25_PROTOCOL_ID    0xF0 /* No third level protocol */

int aprs(void) {
  char addresses[50];
  char information[50];
  uint32_t i = 0;
  uint16_t fcs;

  /* Encode the destination / source / path addresses */
  uint32_t addresses_len = sprintf(addresses, "%-6s%c%-6s%c%-6s%c",
                                   "APRS", 0,
                                   "Q0QQQ", 2,
                                   "WIDE2", 1);
  uint32_t information_len = 5;
  strcpy(information, "HELLO");

  /* Process addresses */
  for (i = 0; i < addresses_len; i++) {

    if ((i % 7) == 6) {         /* Secondary Station ID */
      ax25_frame[i] = ((addresses[i] << 1) & 0x1F) | 0x60;
    } else {
      ax25_frame[i] = (addresses[i] << 1);
    }
  }
  ax25_frame[i-1] |= 0x1;     /* Set HLDC bit */

  ax25_frame[i++] = AX25_CONTROL_WORD;
  ax25_frame[i++] = AX25_PROTOCOL_ID;

  /* Process information */
  memcpy(ax25_frame+i, information, information_len);
  i += information_len;

  return i;
}

int main()
{
  if (wiringPiSPISetup(0, SPI_SPEED) < 0) {
    fprintf(stderr, "Failed to open SPI port.  Try loading spi library with 'gpio load spi'");
  }

  ax_packet rx_pkt;
  uint8_t tx_pkt[0x100];

  ax_config config;
  memset(&config, 0, sizeof(ax_config));

  config.clock_source = AX_CLOCK_SOURCE_TCXO;
  config.f_xtal = 16369000;

  config.synthesiser.A.frequency = 434600000;
  config.synthesiser.B.frequency = 434600000;

  config.spi_transfer = wiringpi_spi_transfer;

  config.pkt_store_flags = AX_PKT_STORE_RSSI |
    AX_PKT_STORE_RF_OFFSET;

  ax_init(&config, &gmsk_modulation);

  ax_tx_on(&config, &gmsk_modulation);
  while (1) {
    /* int aprs_len = aprs(); */
    /* ax_tx_packet(&config, &msk1_modulation, ax25_frame, aprs_len); */

                        ////////////////////////////////////////
    strcpy((char*)tx_pkt, "ughdffgiuhdfudshfdjshfdjshfsudhfdskjfdfd");
    ax_tx_packet(&config, &gmsk_modulation, tx_pkt, 40);
  }

  ax_rx_on(&config, &gmsk_modulation);
  while (1) {
    while (ax_rx_packet(&config, &rx_pkt)) {
      printf("rx!\n");
    }
  }

  return 0;
}
