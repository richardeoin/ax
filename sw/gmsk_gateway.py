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
import habitat
import logging
import yaml

# load gateway parameters from gateway.yaml
try:
    with open("gateway.yaml", 'r') as g:
        gw = yaml.load(g.read())
except:
    raise RuntimeError("Couldn't load gateway.yaml! cp gateway-example.yaml gateway.yaml to get started!")


# TODO set frequency/mode dynamically
frequency_MHz = 434.6375
radio = AxRadioGMSK(spi=0, frequency_MHz=frequency_MHz, mode='Y')


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
    print(doc_type)
    print(doc_id)

# habitat
uploader.initialised = uploader_initialised
uploader.saved_id = uploader_saved_id
uploader.settings(gw["callsign"])
uploader.start()

# Telemetry Metadata
metadata = {
    "frequency": int(frequency_MHz * 1e6)
}


# Receive Loop
def rx_callback(data, length):
    print(length)

    string = data[:-2].decode('utf-8')
    print(string)

    uploader.payload_telemetry(string, metadata=metadata)


radio.receive(rx_callback)
