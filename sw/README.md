# AX

Sofware for controlling the AX5043 and AX5243 radios.

Both C and Python APIs are supported. Whilst the [C API](ax/C-API.md)
provides more detailed control, it has a much steeper learning curve
and requires a greater understand of the radio's internal
operation. Therefore the Python API is recommended where possible.

## Supported Modes

### Transmit

* (G)FSK
* (G)MSK
* AFSK
* CW

## Receive

* (G)FSK
* (G)MSK

Issues with baud rates below 2000 baud.

## Usage

```python
from ax_radio import AxRadio

radio = AxRadio()

# transmit
radio.transmit("Hello World de Q0QQQ "*10)

# receive
def rx_callback(data, length):
    print(data.decode('utf-8'))

radio.receive(rx_callback)
```


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
