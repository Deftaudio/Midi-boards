# Raspberry Pi 4 Eurorack DIY kit

Deftaudio EuroRPi offers missing pieces to assemble RPi 4 with 5" touchscreen LCD into 26HP Eurorack module. It comes with two primary components:
- an aluminum faceplate with holes for LCD, USB ports and TRS breakout;
- a Synth Kit I/O - RPi shield which includes Audio Out, MIDI I/O and power interfacing to Eurorack power.  

![EuroRPi assembled photo](https://github.com/Deftaudio/Midi-boards/raw/master/Eurorack_RPi/Eurorack_RPi_assembled_front1.JPG)

### Introduction Video

https://youtu.be/O-rQi9Hbrpc


### Components selection:
The kit is designed around the idea to use off-the-shelve components to drive the cost down where it's applicable. It's heavily relies on the external sources. Faceplate has a cut for specific type of LCD, based on the DSI interface. This allows to avoid the complexity of HDMI cables, external power and USB port for touchscreen, - it all comes inside DSI port which connects with a flex cable. Faceplate also includes pre-drilled holes for USB extender cables and 3.5mm TRS breakout. There are several recommendations for mounting and cooling solutions for RPI. Those are covered in details in HW assembly guide and BOM reference list.

### Synth Kit I/O:
Synth Kit I/O is a RPi shield which combines three functional blocks:
- Interfacing to Eurorack 16-pin power 5V or +12V with onboard DC-DC converter
- Audio DAC option with Burr Brown 5102A up to 192kHZ, I2S DAC
- MIDI over UART interface with input and output

Synth Kit is very modular, user can pick what components to populate for desired functionality.  
The detailed SW guide covers how to enable audio and MIDI components in Raspbian or PatchBox OS.


  ### BOM list:
https://docs.google.com/spreadsheets/d/15D16pYKxWzvwlihMUm6H49rWIW-DiubtF_ozgL0KMLI/edit?usp=sharing
*Links are for the reference only. You don't need to buy from this suppliers as all parts are widely available in many places.*  

**ASSEMBLY VIDEO:**

https://youtu.be/KfUyIja2pYk

### HW Assembly Guide

https://github.com/Deftaudio/Midi-boards/raw/master/Eurorack_RPi/Eurorack_RPi_synth_kit_hardware_installation.pdf

### Software Installation guide:
https://github.com/Deftaudio/Midi-boards/raw/master/Eurorack_RPi/Eurorack_RPi_synth_kit_software_installation.pdf

### Github file repository:
https://github.com/Deftaudio/Midi-boards/tree/master/Eurorack_RPi


### Purchasing

Tindie: https://www.tindie.com/stores/deftaudio/
eBay: https://www.ebay.com/sch/racoona_kate/m.html?item=265103832897

Andrei Kudryavtsev, Deftaudio 2008-2021.
