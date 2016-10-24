# Python for controlling AX radio
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

from _ax_radio import lib,ffi

class AxRadio:
    def __init__(self, channel=0, frequency=434600000):

        self.config = ffi.new('ax_config*')
        self.mod = ffi.new('ax_modulation*')

        # attempt to open the SPI port
        channel_status = lib.ax_set_spi_transfer(self.config, channel)

        if channel_status == lib.AX_SET_SPI_TRANSFER_FAILED:
            raise RuntimeError('Failed to open SPI port.')
        elif channel_status == lib.AX_SET_SPI_TRANSFER_BAD_CHANNEL:
            raise ValueError('Bad channel number. Try 0 or 1')

        # default configuration for our hardware
        self.config.clock_source = lib.AX_CLOCK_SOURCE_TCXO
        self.config.f_xtal = 16369000

        # set frequencies
        self.config.synthesiser.A.frequency = frequency
        self.config.synthesiser.B.frequency = frequency
        #self.config.pkt_store_flags = lib.AX_PKT_STORE_RSSI | \
        #                         lib.AX_PKT_STORE_RF_OFFSET

        # actually initialise the radio
        init_status = lib.ax_init(self.config)

        if init_status == lib.AX_INIT_BAD_SCRATCH or \
           init_status == lib.AX_INIT_BAD_REVISION:
            raise RuntimeError('Read bad scratch or revision value. '
                               'Is the radio plugged in correctly?')

        self.in_transmit_mode = False

        # set default modulation parameters
        self.modulation()


    def modulation(self, bitrate=2000):
        self.mod.modulation = 8
        self.mod.encoding = 3
        self.mod.framing = 20
        self.mod.bitrate = bitrate
        self.mod.fec = 0
        self.mod.power = 0.1
        self.mod.parameters.fsk.modulation_index = 2/3
        self.mod.continuous = 1


    def transmit(self, bytes_to_transmit): # transmit
        if self.in_transmit_mode == False:
            lib.ax_tx_on(self.config, self.mod)
            self.in_transmit_mode = True

        lib.ax_tx_packet(self.config, bytes_to_transmit, len(bytes_to_transmit))

    def receive(self):              # receive
        lib.ax_rx_on(self.config, self.mod)

if __name__ == "__main__":
    radio = AxRadio()
    radio.receive()
