@page page_guide_rpi_wiegand Raspberry Pi 1 and Wiegand

This page describes how to set up a Raspberry 1 with a Wiegand reader, step by step.
It is not the recommended approach in production but can be useful in lab for tests
purpose. We recommend to use dedicated PCB hardware connected for the Raspberry Pi
(or any other pico-pc) or to go with @subpage page_guide_rpi_piface_wiegand

Informations
============

General infos

 * We use GPIO pins *14* and *15* for the Wiegand reader data transmission wires, respectfully for *Data0* and *Data1*.
 * We use GPIO pin *3* to light up the LED (meaning the door is open).
 * Wiegand reader *Data0* and *Data1* generally output +5V. Raspberry Pi's GPIO voltage levels are 3.3 V.
 * We need a total of 4 wires going to the Raspberry Pi's GPIOS. Last one is for *ground*.

Here is the Raspberry Pi GPIO layout for informations:
![Raspberry pin-out](raspberry-pi-rev2-gpio-pinout.jpg)

Voltage Divider
---------------

A Wiegand reader will usually use +5V as its "high" voltage level. However, Raspberry Pi's pins are built to deal with 3.3V current. Therefore we need to reduce the voltage in order to not damage the pins.

We want to go from +5V to about +3.3V: `Vin = +5` and we would like `Vout = +3.3`.

![Wiegand Wiring](wiegand.png)

The values on the resistors are not strictly fixed. As long as the ratio between the upper and lower resistor value is 1/2 for the [voltage divider](http://en.wikipedia.org/wiki/Voltage_divider), the 5k resistor can be safely replaced with another in the range from 2k to 10k depending on what you have. If you only have 10k resistors, you could even put 2 in series to obtain 20k.

Do not forget to plug the reader on a 12V current source. 5V is also suitable with some readers if you don't have another choice but is less recommended.

DYI Guide
=========

Step 1: Prepare the GPIOs
-------------------------
Raspberry Pi with wires connected to GPIO.

![Raspberry Pi GPIOs](wiegand_voltdivider1.jpg)

 * The black wire is connected to the *ground* GPIO pin.
 * The blue wire goes to GPIO 3.
 * The green wire is connected to GPIO 14.
 * The white wire is connected to GPIO 15.

Step 2: Prepare the breadboard
------------------------------
We will use 2 resistors type: `R1 = 330` and `R2 = 560`. This gives `Vout = 3.146`, which is acceptable.

This is what the breadboard look like, with 4 resistors for voltage divider. The led will be lighted if an access control is successful.
![Breadboard](wiegand_voltdivider2.jpg)

Step 3: The wiegand reader
--------------------------
![Wiegand reader](wiegand_voltdivider3.jpg)

 * The blue, yellow and orange wires are useless, as far as i know.
 * The red wire is for powering the reader. (Based on your reader, it can range from 5V to 12V -- maybe more)
 * The green wire is *Data0* (aka *data -*)
 * The white wire is *Data1* (aka *data +*)
 * No color wire is a ground that needs to be connected to the raspi's ground.

The reader could be powered by the Raspi's 5V pin, but we use an external powering source.

Step 4: Plug in the raspi's wires into the board
------------------------------------------------
We then plug the raspberry pi's wire (connected to the GPIOs) into the board. This will allow us to retrieve data from the reader as well as powering the led when we need to.
![Plug Raspberry with Breadboard](wiegand_voltdivider4.jpg)

Step 5: Plug the reader into the board
------------------------------------------------
![Plug Reader with Breadboard](wiegand_voltdivider5.jpg)

You're done for the hardware setup. If Leosac is already [installed](@ref install_guide), you can now setup the @subpage page_module_sysfs_gpio and @subpage page_module_wiegand.


Better Prototyping
==================

You may want to use *74LVC244* / *74LVC245* or *CD4049* / *CD4050* CMOS devices instead to convert 5V signal to 3.3V in a more professional way.

When working with a breadboard you may even want to use *Level-Shifter 8-Channel*  (or 4 channel, just be sure input is 5V and output 3.3V).
![Level-Shifter 8-Channel](level-shifter-8-channels-bi-directionnal.jpg)