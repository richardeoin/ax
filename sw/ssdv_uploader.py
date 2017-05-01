# Upload to ssdv server
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

import requests
import json
import threading
import time
import strict_rfc3339
import base64

class SSDVUploader:

    def __init__(self, callsign, url="http://ssdv.bristol-seds.co.uk/data"):
        self.callsign = str(callsign)
        self.url = str(url)
        self.batch = []

    def post_data(self, packet):
        to_rfc3339 = strict_rfc3339.timestamp_to_rfc3339_localoffset

        return {
            "type": "packet",
            "packet": base64.b64encode(packet).decode('ascii'),
            "encoding": "base64",
            "received": to_rfc3339(int(round(time.time()))),
            "receiver": self.callsign
        }

    def ssdv_post(self, packet):

        if type(packet) is not list:
            data = self.post_data(packet)
        else:
            data = {
                "type": "packets",
                "packets": [self.post_data(p) for p in packet]
            }

        headers = {
            'Accept': 'application/json',
            'Content-Type': 'application/json'
        }

        requests.post(self.url, headers=headers, data=json.dumps(data))

    def ssdv_post_async(self, packet):
        threading.Thread(target=self.ssdv_post, args=(packet,)).start()

    def ssdv_flush_batch_async(self):
        if self.batch:
            self.ssdv_post_async(self.batch)
            self.batch = []

    def ssdv_post_batch_async(self, packet):
        self.batch.append(packet)

        if len(self.batch) >= 10:
            self.ssdv_flush_batch_async()

if __name__ == "__main__":

    ssdv = SSDVUploader("QQ0QQQ")

    for i in range(20):
        ssdv.ssdv_post_batch_async(b'data'*256)
