@page page_guide_rpi_piface_wiegand Installation Guide (part 2): Raspberry Pi (1 B+ or 2 or 3 or 4) + Piface Digital (1 or 2) + Wiegand Reader

Installation guide {#install_guide_rpi}
=====================================

@brief Describe the additional installation process required with such hardware. You should have first followed @ref page_guide_raspbian.

Additional Hardware {#install_guide_req_hard}
----------------------------------

1. A Raspberry Pi
2. A Piface Digital board (see [here](http://www.piface.org.uk/products/piface_digital/))

Adding modules {#install_guide_rpi_add_mod}
=======================================

We need a few modules to make thing works:
+ Since we are using the PifaceDigital card, we need Leosac's [piface digital](@ref mod_piface_main) module.


Configuring the Piface Digital module {#install_guide_rpi_add_mod_piface}
---------------------------------------------------------------------

You can find the documentation of the module [here](@ref mod_piface_main). It focus on how to configure
the module for any use case. We will instead explain why we need a given configuration.

We are using a Wiegand card reader. Therefore we need 2 INPUTs GPIO.
We also have 1 OUTPUT gpio for the door.

We add this to our config file:

~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>PIFACEDIGITAL_GPIO</name>
    <file>libpifacedigital.so</file>
    <level>2</level>
    <module_config>
        <gpios>
            <gpio>
                <name>my_door_gpio</name>
                <no>3</no>
                <direction>out</direction>
                <value>false</value>
            </gpio>
            <gpio>
                <name>wiegand_data_high</name>
                <no>4</no>
                <direction>in</direction>
            </gpio>
            <gpio>
                <name>wiegand_data_low</name>
                <no>2</no>
                <direction>in</direction>
            </gpio>
        </gpios>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~

As you can see, we are defining GPIO using the gpio port `<no>` we wired our reader and door.

Note that we use `<level>2</level>` to make sure this module is one of the first to start.
We are assigning meaningful name to our GPIO. Keep them in mind because we will reuse them later.

Wiring everything {#install_guide_rpi_wiring}
=========================================

This is the board with the PiFace plug onto it.
![Raspi and board](piface-front.png)
The orange port are output ports. Green ports are for inputs.

We have 3 wire to connect:
    1. Brown wire is our OUTPUT wire, connected to the door.
    2. Purple wire is wiegand low.
    3. Yellow wire is wiegand high.

![Raspi wired](piface-wire.png)


Conclusion {#install_guide_rpi_end}
===============================

This is theoretically easy and should work.
The whole configuration file can be found in `cfg/example/guide_rpi_piface_wiegand/`.
