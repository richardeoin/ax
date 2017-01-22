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
from enum import Enum
import time

class AxRadio:
    Modulations = Enum('Modulation', 'FSK MSK GFSK GMSK PSK AFSK CW')
    VcoTypes = Enum('VcoType', 'Undefined Internal Inductor External')

    def __init__(self,
                 spi=0, vco_type=VcoTypes.Undefined,
                 frequency_MHz=434.6, modu=Modulations.FSK,
                 bitrate=20000, fec=False, power=0.1, cont=True):

        self.config = ffi.new('ax_config*')
        self.mod = ffi.new('ax_modulation*')

        # attempt to open the SPI port
        spi_status = lib.ax_set_spi_transfer(self.config, spi)

        if spi_status == lib.AX_SET_SPI_TRANSFER_FAILED:
            raise RuntimeError('Failed to open SPI port.')
        elif spi_status == lib.AX_SET_SPI_TRANSFER_BAD_SPI:
            raise ValueError('Bad spi number. Try 0 or 1')

        # default configuration for our hardware
        self.config.clock_source = lib.AX_CLOCK_SOURCE_TCXO
        self.config.f_xtal = 16369000

        # set frequencies
        self.config.synthesiser.A.frequency = int(frequency_MHz * 1e6)
        self.config.synthesiser.B.frequency = int(frequency_MHz * 1e6)
        if vco_type == self.VcoTypes.Undefined: # guess VCO type
            if frequency_MHz > 400:
                vco_type = self.VcoTypes.Internal
            else:
                vco_type = self.VcoTypes.Inductor
        if vco_type == self.VcoTypes.Inductor:
            self.config.synthesiser.vco_type = lib.AX_VCO_INTERNAL_EXTERNAL_INDUCTOR
        elif vco_type == self.VcoTypes.External:
            self.config.synthesiser.vco_type = lib.AX_VCO_EXTERNAL

        # report rssi and rf frequency offset
        self.config.pkt_store_flags = lib.AX_PKT_STORE_RSSI | \
                                lib.AX_PKT_STORE_RF_OFFSET

        # actually initialise the radio
        init_status = lib.ax_init(self.config)

        if init_status == lib.AX_INIT_BAD_SCRATCH or \
           init_status == lib.AX_INIT_BAD_REVISION:
            raise RuntimeError('Read bad scratch or revision value. '
                               'Is the radio plugged in correctly?')
        elif init_status == lib.AX_INIT_VCO_RANGING_FAILED:
            raise RuntimeError('VCO ranging failed. Try a different frequency '
                               'or changing `vco_type` and `rf_div`')

        self.in_transmit_mode = False

        # set modulation parameters
        self.modulation(bitrate, modu, fec, power, cont)

        # calculate tweakable parameters
        lib.ax_default_params(self.config, self.mod)


    def modulation(self, bitrate, modu, fec, power, cont):
        if modu == self.Modulations.FSK or modu == self.Modulations.GFSK:
            self.mod.modulation = lib.AX_MODULATION_FSK
        if modu == self.Modulations.MSK or modu == self.Modulations.GMSK:
            self.mod.modulation = lib.AX_MODULATION_MSK
        if modu == self.Modulations.PSK:
            self.mod.modulation = lib.AX_MODULATION_PSK
        if modu == self.Modulations.AFSK:
            self.mod.modulation = lib.AX_MODULATION_AFSK
        if modu == self.Modulations.CW:
            self.mod.modulation = lib.AX_MODULATION_CW

        # fec and framing
        if fec:                 # forward error correction
            self.mod.fec = 1
            self.mod.encoding = lib.AX_ENC_NRZ | lib.AX_ENC_SCRAM
            # HDLC required for FEC
            self.mod.framing = lib.AX_FRAMING_MODE_HDLC | \
                               lib.AX_FRAMING_CRCMODE_CCITT

        else:                   # nrzi with pattern match
            self.mod.fec = 0
            self.mod.encoding = lib.AX_ENC_NRZI
            self.mod.framing = lib.AX_FRAMING_MODE_RAW_PATTERN_MATCH | \
                               lib.AX_FRAMING_CRCMODE_CCITT

        # gaussian shaping
        if modu == self.Modulations.GFSK or modu == self.Modulations.GMSK:
            self.mod.shaping = lib.AX_MODCFGF_FREQSHAPE_GAUSSIAN_BT_0_5

        self.mod.bitrate = bitrate

        if cont:
            self.mod.continuous = 1
        else:
            self.mod.continuous = 0

        if (power <= 1) and (power >= 0):
            self.mod.power = power
        else:
            self.mod.power = 0.1

        # fsk: modulation index
        if modu == self.Modulations.FSK or modu == self.Modulations.GFSK:
            self.mod.parameters.fsk.modulation_index = 2/3

        # afsk: deviation, mark, space
        if modu == self.Modulations.AFSK:
            # set to reasonable APRS values
            self.mod.parameters.afsk.deviation = 3000
            self.mod.parameters.afsk.space = 2200 # bell 202
            self.mod.parameters.afsk.mark  = 1200


    def transmit(self, bytes_to_transmit): # transmit
        if self.in_transmit_mode == False:
            lib.ax_tx_on(self.config, self.mod)
            self.in_transmit_mode = True

        lib.ax_tx_packet(self.config, self.mod,
                         bytes_to_transmit, len(bytes_to_transmit))


    def receive(self, rx_func): # receive
        pkt = ffi.new('ax_packet*')

        lib.ax_rx_on(self.config, self.mod)
        self.in_transmit_mode = False

        while 1:
            while lib.ax_rx_packet(self.config, pkt): # empty the fifo
                data = ffi.string(pkt.data[0:pkt.length])
                metadata = {
                    'rssi': pkt.rssi,
                    'rffreqoffs': pkt.rffreqoffs,
                }
                if rx_func:
                    rx_func(data, pkt.length, metadata)

            time.sleep(0.025)         # 25ms sleep

    def get_modulation(self):       # getter
        return self.mod


"""
GMSK-{X,Y,Z} modes
"""
class AxRadioGMSK(AxRadio):
    def __init__(self,
                 spi=0, vco_type=AxRadio.VcoTypes.Undefined,
                 frequency_MHz=434.6, mode='X', power=0.1):

        if mode == 'X' or mode == 'x':
            bitrate = 12000
        elif mode == 'Y' or mode == 'y':
            bitrate = 24000
        elif mode == 'Z' or mode == 'z':
            bitrate = 115000
        else:
            raise RuntimeError('Unknown mode \'{}\'!'.format(mode))

        # configure radio
        AxRadio.__init__(self, spi, vco_type, frequency_MHz,
                         modu=AxRadio.Modulations.GMSK,
                         bitrate=bitrate, fec=True, power=power)

"""
APRS
"""
class AxRadioAPRS(AxRadio):
    def __init__(self,
                 spi=0, vco_type=AxRadio.VcoTypes.Undefined,
                 frequency_MHz=434.6, power=0.1, deviation=3000):

        # configure radio
        AxRadio.__init__(self, spi, vco_type, frequency_MHz,
                         modu=AxRadio.Modulations.AFSK,
                         bitrate=1200, fec=False, power=power, cont=False)

        # set new deviation
        self.mod.parameters.afsk.deviation = deviation

        # re-calculate tweakable parameters
        lib.ax_default_params(self.config, self.mod)


if __name__ == "__main__":

    def rx_callback(data, length, metadata):
        print(length)
        print(data[:-2].decode('utf-8'))

    radio = AxRadio()

    #radio.transmit("Hello World de Q0QQQ "*10)

    radio.receive(rx_callback)
