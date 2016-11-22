# Hardware for ax-chip

This folder contains the schematic files for the ax-chip header
board.

The board uses a single AX5043 radio, with separate antenna ports for
transmit and receive. The recieve port has an integrated Low Noise
Amplifier (LNA). *Not yet tested*

The LNA used is an
[Avago MGA-62563](http://www.avagotech.com/docs/AV02-1237EN), but a
MGA-61563 or MGA-68563 could also be used, by adjusting the bias
resistor R12. By default the MGA-62563 is biased to 3.3V, 30mA.

| Frequency (MHz) | Theoretical NF @ 25ºC |
|---|---
| 144 | 1.3
| 434 | 0.9

The differential match following the LNA is tuned to for both 144MHz
and 434MHz (blue line in this [plot](144_434_S21.png)). The single
ended match on the transmit port is tuned for 144MHz, although other
values can be found in the AX5043 datasheet.

`Lext` can be populated so that frequencies outside of 400-525MHz /
800-1050MHz can be used; usually this is `22nH` for 140-154MHz.

_[Schematics](ax-chip.sch.pdf)_

_[Board Drawings](ax-chip.kicad_pcb.pdf)_

_[3D Rendering](ax-chip.kicad_pcb.png)_

<!-- ![Assembled ax-gateway header board](ax-gateway-on-pi.jpg -->
<!--  "Assembled ax-gateway header board") -->
