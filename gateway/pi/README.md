# ax-gateway for raspberry pi

uses wiring pi

* checks for AX5043 / AX5243 on SPI channels 0 and 1
* TODO: initialisation
* TODO: transmit
* TODO: receive

### File Structure

* ax.{c,h} - general chip control functions, same for all platforms
* ax_hw.{c,h} - spi hardware functions, `ax_hw_{read,write}_register`
  and so on. Specific to wiring-pi
* ax_test.c - test for pi
* ax_reg.h - register addresses
* ax_reg_values.h - register values

### API

#### `ax_init(ax_config* config)`

* checks spi interface is functional and ax chip present
* switch to STANDBY mode
* starts up oscilator and performs VCO ranging
* switch to POWERDOWN/DEEPSLEEP mode

#### `ax_adjust_frequency(ax_config* config, enum ax_pll pll, uint32_t frequency)`

* check frequency change is less than 5MHz in 868/915 or 2.5MHz in 433
* re-calculate register values for new frequency
* write new values to chip (if not in DEEPSLEEP)

#### `ax_tx_on(ax_config* config, ax_modulation* modulation)`

* set parameters for given modulation
* switch to FULLTX mode
* re-runs autoranging if PLL fails to lock

#### `ax_tx_packet(ax_config* config, uint8_t* packet, uint16_t length)`

* checks for FULLTX mode
* loads packet into FIFO

#### `ax_rx_on(ax_config* config, ax_modulation* modulation)`

* sets parameters for given modulation
* switch to FULLRX mode
* re-runs autoranging if PLL fails to lock

#### `ax_rx_wor(ax_config* config, ax_modulation* modulation)`

* set parameters for given modulation
* set parameters for WOR
* switch to WOR mode
* re-runs autoranging if PLL fails to lock

#### `ax_off(ax_config* config)`

* switch to POWERDOWN/DEEPSLEEP mode

## Notes

In HDLC mode, bit stuffing seems to affect the AFC loop. In particular
the value of TRK RF FREQ is affected. If the AFC loop is disabled (MAX
RF OFFSET = 0) then the receiver will struggle with bit-stuffed packets.

There's minimum IF frequencies - 3.18kHz for FSK modes and 9.38kHz for
ASK/PSK. These might reduce sensitivity at small bandwidths!
