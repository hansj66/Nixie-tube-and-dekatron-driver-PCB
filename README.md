Nixie-tube-and-dekatron-driver-PCB
==================================

Description

This is the schematic and PCB layout for a driver board capable of supporting up to 6 Nixie tubes and an OG-4 dekatron.

The example firmware illustrates how you can control the clock via the UART. The example demonstrates a clock and a generic display mode.

If you want, you can plug a BlueSMiRF Bluetooth modem directly into the board for wireless control of the clock

Instructions

This is a project I finished a couple of years ago after a Nixie tube buying spree on eBay. I ended up constructing a single clock before moving on to the next project.

Since I haven't seen too many Nixie clock PCBs here, I hope that someone might find the schematic useful. It can at least serve as a demonstrator on how to control dekatrons and nixie displays via a microcontroller.

Description of the files:

NixieClock.T3001 is a Target 3001 project file with schematic and routed PCB layout
NixieClock.SCR is an Eagle export of the Target 3001 project
Gerber.zip contains milling/drilling files for PCB production.
The PDF contains a parts list, board pinout and the command set which is implemented in the example firmware.
firmware_example.c is an example on how to drive the clock. (It is newbie AVR C-style code, so I suggest you mabye write your own firmware for it ;))
Suggestions for improvements for anyone who wants to hack this: 

1) Battery stand by 

2) Sleep mode with display and driver IC enable/disable function. 

3) Rip out the microcontroller and re-create it as an Arduino shield.

You will need an external high voltage power source to drive the tubes. You can usually get these from ebay pretty cheap.If you are feeling brave, you can of course try to modify http://www.thingiverse.com/thing:19687 to get the voltages you need for driving the tubes. (If I remember correctly, the dekatron requires 450V and the generic Nixie tubes approximately 170V)

You should never use a non-isolated power supply for powering something like this. (I vapourized a PCB in an early attempt to drive these tubes with a non-isolated power supply. Fortunatley, the main circuit breaker in my house triggered before something caught fire...)

PS. There is no source files / model for the casing. It was cast in concrete in a hand crafted mold. If you are interested, you can find more pictures of the clock on my blog: http://www.timeexpander.com/wordpress/archives/2540

