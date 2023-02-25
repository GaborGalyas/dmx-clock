# DMX controlled Clock
The files here are for making your own DMX controlled clock. Board files, schematics, and source codes can be found here.
# File descriptions:
## 2pix.brd
Printed Circuit Board file created with EAGLE 6.5.0
## 2pix.sch
Schematic file created with EAGLE 6.5.0
## 2pix_top_bom and 2pix_top_cpl 
Bill Of Material and component files what you can upload to a PCB manufacturing site to have your boards assembled
## GGL-CLOCK-FRAME.stl
A 3D printable frame for 2 modules. You need to round the corners of the pcb-s to fit in the corners of the frame. Add a plexiglass to the front with a sheet of frost paper or parchment paper / baking paper between the two. Use M3 bolts from the front to fix the plexi/frost/frame/pcb sandwich together.
## clock_gerbers.zip
Upload this file to a pcb manufacturer site like JLCPCB to have xour boards made.
## ggl_clock_dmx_2col.ino
This code is written for an arduino mini with wired dmx input. 
This will need tinkering, you need to connect the arduino to the modules and implement the dmx receiving circuit.
I will work on making a pcb design for a drop in replacement for the wemos d1 mini.
## wireless_clock_wifimanager_2col.ino
This version is for a wemos D1 mini. After assembling the pcb-s solder the wemos d1 mini to the first module, and upload code. For configuration short the D7 pin to ground on boot, and the wifi manager will load. You will be presented with a wireless hotspot named "GGL ARTNET CLOCK CONFIG ME". After connecting, you can configure the wifi network that has Art-Net data coming from the lighting network, and set the dmx address too. Once saved, on every boot the clock will try to connect to the network you provided.
