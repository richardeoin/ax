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
import habitat
from datetime import datetime

# TODO set dynamically
callsign = "test_r1"
lat = 1.0
lon = 1.0
alt = 1
location = "Somewhere"
radio_name = "Ax"
antenna = "Unknown"


# TODO set frequency/mode dynamically
radio = AxRadioGMSK(spi=0, frequency_MHz=434.6, mode='Y')


# Habitat
uploader = habitat.uploader.Uploader(callsign)

time_str = datetime.utcnow().strftime("%H:%M:%S")
uploader.listener_telemetry({
    "time": time_str,
    "latitude": lat,
    "longitude": lon,
    "altitude": alt
})

uploader.listener_information({
    "name": callsign,
    "location": location,
    "radio": radio_name,
    "antenna": antenna
})


# Receive Loop

def rx_callback(data, length):
    print(length)
    print(data[:-2].decode('utf-8'))

radio.receive(rx_callback)
