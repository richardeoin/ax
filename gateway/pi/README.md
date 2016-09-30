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
