# Hardware for ax-gateway

This folder contains the schematic files for the ax-gateway header
board.

This board supports one or two AX5243 radios, with a bi-directional
antenna port for each radio. It has been tested with a
[Raspberry Pi 3 Model B](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/).

The differential match can be populated for various frequencies, using
the values found in the AX5243 datasheet. Usually this is 144MHz,
434MHz or 868MHz. `Lext` can be populated so that frequencies outside
of 400-525MHz / 800-1050MHz can be used; usually this is `22nH` for
140-154MHz.

_[Schematics](ax-gateway.sch.pdf)_

_[Board Drawings](ax-gateway.kicad_pcb.pdf)_

_[3D Rendering](ax-gateway.kicad_pcb.png)_

![Assembled ax-gateway header board](ax-gateway-on-pi.jpg
 "Assembled ax-gateway header board")
