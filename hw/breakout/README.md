# Hardware for ax-breakout

This folder contains the schematic files for the ax-breakout header
board.

This board breaks out a single AX5243 radio to 2mm pitch pins. *Not yet tested*

The differential match can be populated for various frequencies, using
the values found in the AX5243 datasheet. Usually this is 144MHz,
434MHz or 868MHz. `Lext` can be populated so that frequencies outside
of 400-525MHz / 800-1050MHz can be used; usually this is `22nH` for
140-154MHz.

_[Schematics](ax-breakout.sch.pdf)_

_[Board Drawings](ax-breakout.kicad_pcb.pdf)_

_[3D Rendering](ax-breakout.kicad_pcb.png)_

![Rendering of ax-breakout board](ax-breakout.kicad_pcb.png
 "Rendering of ax-breakout board")
