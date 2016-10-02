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

#include "ax.h"
#include "ax_hw.h"
#include "ax_modes.h"

int main()
{
  ax_hw_init(0);
  ax_hw_init(1);

  uint8_t pkt[0x100];

  ax_config config;
  memset(&config, 0, sizeof(ax_config));

  config.clock_source = AX_CLOCK_SOURCE_TCXO;
  config.f_xtal = 16369000;

  config.synthesiser.A.frequency = 434600000;
  config.synthesiser.B.frequency = 434600000;


  ax_init(&config);
  ax_tx_on(&config, &psk1_modulation);

  while (1) {
    sleep(1);

    strcpy((char*)pkt, "-hello");
    ax_tx_packet(&config, pkt, 5);
  }

  return 0;
}
