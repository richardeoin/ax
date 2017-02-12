# AX on CHIP

## Before you start

* Flash. I used 4.4 command line version. `uname -a` gives `Linux chip
  4.4.13-ntc-mlc #1 SMP Tue Dec 6 21:38:00 UTC 2016 armv7l GNU/Linux`
* Setup wifi etc.
* Disable low power wifi because it's annoying `sudo iw wlan0 set power_save off`

## Enable SPI

There's some automatic way of doing this with the EEPROM, but for now
do it manually.

From
[https://bbs.nextthing.co/t/spi-serial-communication-on-chip/11937/7](https://bbs.nextthing.co/t/spi-serial-communication-on-chip/11937/7):

open startup script for editing
```
sudo nano /etc/rc.local
```

add the following before `exit 0`

```
mkdir -p /sys/kernel/config/device-tree/overlays/spi
cat /lib/firmware/nextthingco/chip/sample-spi.dtbo > /sys/kernel/config/device-tree/overlays/spi/dtbo
```

close nano and restart `sudo shutdown -r now`.

once you've rebooted `/dev/spidev32766.0` should now exist.

## Build

```
# get cffi
sudo apt-get install git python-pip python-dev libffi-dev
sudo pip install cffi enum34

# build ax for chip
python ax_build_chip.py
```
