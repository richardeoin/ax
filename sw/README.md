# AX

Sofware for controlling the AX5043 and AX5243 radios.

Both C and Python APIs are supported. Whilst the [C API](ax/C-API.md)
provides more detailed control, it has a much steeper learning curve
and requires a greater understand of the radio's internal
operation. Therefore the Python API is recommended where possible.

You can also use the Python API to generate an initialised C
structure, which can then be passed to the C API at runtime. This is
useful for microcontrollers where Python is not available or
appropriate, but the full functionality of the C API is not required
either. See [Ax-Structures.md](Ax-Structures.md)

*Changelog:*
* 02/07/16 Push v0.2. rx_callback now requires 3 arguments as example below.

## Supported Modes

### Transmit

* (G)FSK
* (G)MSK
* AFSK
* CW

### Receive

* (G)FSK
* (G)MSK

Issues with (G)MSK below 2000 baud.

## Usage

```python
from ax_radio import AxRadio

radio = AxRadio()

# transmit
radio.transmit("Hello World de Q0QQQ "*10)

# receive
def rx_callback(data, length, metadata):
    print(data[:-2].decode('utf-8'))

radio.receive(rx_callback)
```

More details in [Examples.md](Examples.md).

## Resetting

The following proceedure should be preformed to wake from DEEPSLEEP.

* Set SEL high for at least 1us, then low
* Wait for MISO to go high

## Notes

In HDLC mode, bit stuffing seems to affect the AFC loop. In particular
the value of TRK RF FREQ is affected. If the AFC loop is disabled (MAX
RF OFFSET = 0) then the receiver will struggle with bit-stuffed packets.

There's minimum IF frequencies - 3.18kHz for FSK modes and 9.38kHz for
ASK/PSK. These might reduce sensitivity at small bandwidths!


## Installing

#### Raspberry Pi

Use `raspi-config` to enable SPI!

Then

```
# get cffi
sudo apt-get install git python-pip python-dev libffi-dev
sudo pip install cffi enum34 wiringpi2

# get wiring pi
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build

# build ax for rpi
python ax_build_raspberry_pi.py
```

#### Habitat

Installing habitat on RPi `2016-09-23-raspbian-jessie-lite.img`

```
sudo apt-get install git python-pip python-dev swig libssl-dev
git clone https://github.com/ukhas/habitat
# needed to install m2crypto
sudo ln -s /usr/include/arm-linux-gnueabihf/openssl/opensslconf.h /usr/include/openssl/opensslconf.h
sudo pip install -r requirements.txt
```

TODO: somehow install habitat globally, or just move habitat/habitat to sw

Also need to build reed-solomon library:

```
cd rs8
python build_rs8.py
cd ..
```

Now you can run a gateway:

```
cp gateway-example.yaml gateway.yaml

# edit gateway.yaml
nano gateway.yaml

# run gateway
python gmsk_gateway.py
```

Use `python gmsk_gateway.py --help` for more information.
