#
# Makefile for doing agg-kicad/scripts things
#
# https://github.com/adamgreig/agg-kicad/tree/master/scripts
#

PROJECT_NAME=ax-breakout

all: bom.pdf $(PROJECT_NAME).bom

#
# human-readable BOM file
#
%.bom: %.xml
	python3 /richard/_kicad/agg-kicad/scripts/xml2bom.py $< > $@

#
# stickerbom!
#
bom.pdf: $(PROJECT_NAME).xml
	python3 /richard/_kicad/agg-kicad/scripts/stickerbom.py --suppliers \
		Farnell,RS,DigiKey,Mouser,Uputronics,STD.PAS $< bom.pdf
	mkdir -p bom
	pdftk bom.pdf burst
	mv pg* bom/
