## Python Examples

### FSK

The default is FSK at 20kBaud.

```python
from ax_radio import AxRadio

radio = AxRadio(spi=0,
                frequency_MHz=434.6,
                modu=AxRadio.Modulations.FSK,
                bitrate=20000)

# transmit
radio.transmit("Hello World de Q0QQQ "*10)

# receive
def rx_callback(data, length):
    print(data[:-2].decode('utf-8'))

radio.receive(rx_callback)
```

Other possible modulation values:

* `AxRadio.Modulations.FSK`
* `AxRadio.Modulations.MSK`
* `AxRadio.Modulations.GFSK`
* `AxRadio.Modulations.GMSK`
* `AxRadio.Modulations.AFSK`
