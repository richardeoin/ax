# Script to build _ax_radio.so for the Raspberry Pi
# Copyright (C) 2016  Richard Meadows <richardeoin>

# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:

# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import re
from cffi import FFI
ffibuilder = FFI()

# headers we'd like to use from python
ax_headers = ["ax/ax.h"]
#              "ax/ax.h", "ax/ax_hw.h", "ax/ax_modes.h",
#              "ax/ax_reg.h", "ax/ax_reg_values.h"]

for header in ax_headers:
    header = open(header, 'r').read()
    h = re.sub('[#].*\n', '', header) # remove header guards
    ffibuilder.cdef(h)


# functions to setup spi callbacks
status_enum = """
enum ax_set_spi_transfer_status {
  AX_SET_SPI_TRANSFER_OK,
  AX_SET_SPI_TRANSFER_BAD_CHANNEL,
  AX_SET_SPI_TRANSFER_FAILED,
};
"""
ffibuilder.cdef(status_enum)
ffibuilder.cdef("""
enum ax_set_spi_transfer_status
     ax_set_spi_transfer(ax_config* config, int channel);
""")
spi_callbacks_source = """
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "ax/ax.h"
#define SPI_SPEED	5000000     /* 5MHz */

void wiringpi_spi_transfer_channel_0(unsigned char* data, uint8_t length) {
  wiringPiSPIDataRW(0, data, length);
}
void wiringpi_spi_transfer_channel_1(unsigned char* data, uint8_t length) {
  wiringPiSPIDataRW(1, data, length);
}

enum ax_set_spi_transfer_status
     ax_set_spi_transfer(ax_config* config, int channel)
{
  if (wiringPiSPISetup(channel, SPI_SPEED) < 0) {
    fprintf(stderr, "Failed to open SPI port. Try loading spi library with 'gpio load spi'");
    return AX_SET_SPI_TRANSFER_FAILED;
  }

  if (channel == 0) {
    config->spi_transfer = wiringpi_spi_transfer_channel_0;
  } else if (channel == 1) {
    config->spi_transfer = wiringpi_spi_transfer_channel_1;
  } else {
    return AX_SET_SPI_TRANSFER_BAD_CHANNEL;
  }

  return AX_SET_SPI_TRANSFER_OK;
}
"""

# source files to build
ax_sources = ["ax/ax.c", "ax/ax_hw.c", "ax/ax_modes.c"]
ffibuilder.set_source("_ax_radio", status_enum + spi_callbacks_source,
                      sources=ax_sources, libraries=['wiringPi'], include_dirs=['.'])

# main
if __name__ == "__main__":
    ffibuilder.compile(verbose=True)
