# Upload packets from GMSK radio to habitat
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

from ax_radio import AxRadioGMSK
from datetime import datetime
from rs8 import rs8
import habitat
import logging
import yaml
import argparse
import time

parser = argparse.ArgumentParser(description=
                                 'Gateway from AX radio GMSK modes to habitat.')
parser.add_argument('-f', '--offline', action='store_true',
                    help='do not connect to habitat')
parser.add_argument('-i', '--infodoc', action='store_true',
                    help='print document ids of uploaded documents')
parser.add_argument('-l', '--leds', action='store_true',
                    help='blink LEDs')

args = parser.parse_args()

# load gateway parameters from gateway.yaml
try:
    with open("gateway.yaml", 'r') as g:
        gw = yaml.load(g.read())
except:
    raise RuntimeError("Couldn't load gateway.yaml! cp gateway-example.yaml gateway.yaml to get started!")

# leds
def rx_led(f):
    if "rx_led" in gw:
        f(gw["rx_led"])

def habitat_led(f):
    if "habitat_led" in gw:
        f(gw["habitat_led"])

def do_leds(f):
    rx_led(f)
    habitat_led(f)

if args.leds:
    import wiringpi
    wiringpi.wiringPiSetup()

    def init_led(led):
        wiringpi.pinMode(led, 1)   # set output
        wiringpi.digitalWrite(led, 0) # set off

    do_leds(init_led)

def deinit_leds():
    if args.leds:
        def deinit_led(led):
            wiringpi.pinMode(led, 0)   # set input

        do_leds(deinit_led)

def blink(led):
    if args.leds:
        wiringpi.digitalWrite(led, 1) # set on
        time.sleep(0.002)
        wiringpi.digitalWrite(led, 0) # set off


# TODO set frequency/mode dynamically
frequency_MHz = 434.6375

if not args.offline:            # if online
    # Habitat
    uploader = habitat.uploader.UploaderThread()

    # create logger
    logger = logging.getLogger('habitat.uploader')
    logger.setLevel(logging.DEBUG)

    logfile = datetime.utcnow().strftime("gmsk_gateway_%H_%M_%S.log")
    ch = logging.FileHandler(logfile)
    ch.setLevel(logging.DEBUG)
    logger.addHandler(ch)

    # add listener once UploadThread initialises
    def uploader_initialised():
        time_str = datetime.utcnow().strftime("%H:%M:%S")
        uploader.listener_telemetry({
            "time": time_str,
            "latitude": gw["latitude"],
            "longitude": gw["longitude"],
            "altitude": gw["altitude"]
        })

        uploader.listener_information({
            "name": gw["callsign"],
            "location": gw["location"],
            "radio": "AX",
            "antenna": gw["antenna"]
        })

    def uploader_saved_id(doc_type, doc_id):
        habitat_led(blink)
        if args.infodoc:
            print("{} upload success! id {}".format(doc_type, doc_id))

    # habitat
    uploader.initialised = uploader_initialised
    uploader.saved_id = uploader_saved_id
    uploader.settings(gw["callsign"])
    uploader.start()



# Receive Loop
def rx_callback(data, length, ax_metadata):

    # strip crc
    message = data[:-2]
    length = length - 2

    if length < 32:
        #print("not enough bytes for reed solomon!")
        #print("")
        return

    print("(Length:      {})".format(length))
    print("(RSSI:        {} dBm)".format(ax_metadata['rssi']))
    print("(Freq offset: {} Hz)".format(ax_metadata['rffreqoffs']))

    # Telemetry Metadata
    metadata = {
        "frequency": int((frequency_MHz * 1e6) + ax_metadata['rffreqoffs']),
        "signal_strength": ax_metadata['rssi']
    }

    error_count = rs8.decode_rs_8_assume_pad(message)
    if error_count == -1:
        print("not recoverable with reed-solomon!")
        #print(message)
        print("")
        return

    print("(RS C Errors: {})".format(error_count))

    # decode data to ascii
    try:
        string = message[:-32].decode('ascii')
        print(string)
        rx_led(blink)

        # upload
        try:
            if not args.offline:
                uploader.payload_telemetry(string, metadata=metadata)

                # TODO: uploads to the ssdv server
        except:
            print("Unexpected error uploading packet!")
    except:
        print("Packet string was not valid ascii!")


# start rx
radio = AxRadioGMSK(spi=0, frequency_MHz=frequency_MHz, mode='X',
                    accept_crc_failures=True)

try:
    radio.receive(rx_callback)

except KeyboardInterrupt:
    deinit_leds()

    if not args.offline:
        print("")
        print("Uploading remaining packets to habitat...")
