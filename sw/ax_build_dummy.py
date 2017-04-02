# Script to build _ax_radio.so for a dummy platform with no hardware
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
import sys
from cffi import FFI
ffibuilder = FFI()

# command line args
debug = True if 'debug' in sys.argv else False

# headers we'd like to use from python
ax_headers = ["ax/ax.h"]
for header in ax_headers:
    header = open(header, 'r').read()
    h = re.sub('[#].*\n', '', header) # remove header guards
    ffibuilder.cdef(h)

# definitions we'd like to use from python
defs = ""
for line in open('ax/ax_reg_values.h', 'r'):
    if line.startswith("#define"):
        defs += line        # collect #defines
defs = re.sub('\/\*.*\*\/', '', defs) # strip comments
defs = re.sub('\/\/.*\n', '', defs) # strip comments
# reformat to enum
defs = re.sub(r'#define (AX_[^\s]+)\s+(.*)\n', r'  \1 = \2,\n', defs)
# replace (n<<m) with n*2^m
defs = re.sub(r'\((\d+)\s+<<\s+(\d+)\)',
              lambda m: str(int(m.group(1))*2**int(m.group(2))), defs)
definitions_enum = "enum ax_reg_values {" + defs + "};"
ffibuilder.cdef(definitions_enum)

# functions we'd like to use from python
status_enum = """
enum ax_set_spi_transfer_status {
  AX_SET_SPI_TRANSFER_OK,
  AX_SET_SPI_TRANSFER_BAD_SPI,
  AX_SET_SPI_TRANSFER_FAILED,
};
"""
ffibuilder.cdef(status_enum)
ffibuilder.cdef("""
enum ax_set_spi_transfer_status
     ax_set_spi_transfer(ax_config* config, int spi);
void ax_platform_init(ax_config* config);
""")
spi_callbacks_source = """
#include "ax/ax.h"

void wiringpi_spi_transfer_spi_0(unsigned char* data, uint8_t length) {
  /* dummy */
}

enum ax_set_spi_transfer_status
     ax_set_spi_transfer(ax_config* config, int spi)
{
  /* dummy */
  config->spi_transfer = wiringpi_spi_transfer_spi_0;

  return AX_SET_SPI_TRANSFER_OK;
}

void ax_platform_init(ax_config* config) { /* nothing */ }
"""

compile_args = ["-D_AX_DUMMY", "-D_AX_TX_DIFF"]
if debug:
    compile_args.append("-DDEBUG")

# source files to build
ax_sources = ["ax/ax.c", "ax/ax_hw.c", "ax/ax_modes.c", "ax/ax_params.c"]
ffibuilder.set_source("_ax_radio",
                      definitions_enum + status_enum + spi_callbacks_source,
                      sources=ax_sources, include_dirs=['.'],
                      extra_compile_args=compile_args)



# main
if __name__ == "__main__":
    ffibuilder.compile(verbose=True)
