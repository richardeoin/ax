# ax-gateway for raspberry pi

uses wiring pi

* checks for AX5043 / AX5243 on SPI channels 0 and 1
* transmit and receive

### File Structure

* ax.{c,h} - general chip control functions, same for all platforms
* ax_hw.{c,h} - spi hardware functions, `ax_hw_{read,write}_register`
  and so on.
* ax_params.{c,h} - calculates tweakable parameters
* ax_reg.h - register addresses
* ax_reg_values.h - register values
* ax_fifo.h - constants and structures for FIFO

* ax_test.c - test for pi

### Resetting

The following proceedure should be preformed to wake from DEEPSLEEP.

* Set SEL high for at least 1us, then low
* Wait for MISO to go high

### API

#### `ax_init(ax_config* config)`

* checks spi interface is functional and ax chip present
* switch to STANDBY mode
* starts up oscilator and performs VCO ranging
* switch to POWERDOWN/DEEPSLEEP mode

#### `ax_default_params(ax_config* config, ax_modulation* mod)`

* must have called `ax_init` first
* sets tweakable values in `mod->par` to their default values

#### `ax_adjust_frequency(ax_config* config, uint32_t frequency)`

* check frequency change is less than 5MHz in 868/915 or 2.5MHz in 433
* re-calculate register values for new frequency
* write new values to chip (if not in DEEPSLEEP)
* (currently just frequency synth A)

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

#### `ax_rx_packet(ax_config* config, ax_packet** ptr)`

* checks fifo for packets
* returns 1 if packet was received, 0 otherwise
* should be called in a while loop, to fully empty the FIFO

#### `ax_off(ax_config* config)`

* switch to POWERDOWN/DEEPSLEEP mode

## Notes

In HDLC mode, bit stuffing seems to affect the AFC loop. In particular
the value of TRK RF FREQ is affected. If the AFC loop is disabled (MAX
RF OFFSET = 0) then the receiver will struggle with bit-stuffed packets.

There's minimum IF frequencies - 3.18kHz for FSK modes and 9.38kHz for
ASK/PSK. These might reduce sensitivity at small bandwidths!
