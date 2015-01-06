PifaceDigital Module Documentation {#mod_piface_main}
=====================================================

[TOC]

Introduction {#mod_piface_intro}
=================================

Documentation for the PifaceDigital module.
This provide support for using the PifaceDigital device with Leosac, as a GPIO controller.
This can be very handy because it eases wiring.

This module requires that the Linux Kernel Module SPI be enabled.
You can do this through `raspi-config` on Raspbian.

Configuration Options {#mod_piface_user_config}
================================================

See below for various configuration options.


Options    | Options   | Options                | Description                                            | Mandatory
-----------|-----------|------------------------|--------------------------------------------------------|-----------
gpios      |           |                        | GPIOS definitions                                      | YES
---->      | name      |                        | A name for the GPIO device                             | YES
---->      | no        |                        | The GPIO number of the piface. Range from 0 to 7       | YES
---->      | direction |                        | Direction of the PIN. in or out                        | YES
---->      | value     |                        | Only for out PIN. The default value of the PIN         | YES

Notes:
+ `value` is a boolean. It's only for output GPIO and represents the default value.


Example {#mod_piface_example}
------------------------------


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>PIFACE-GPIO</name>
            <file>libpifacedigital.so</file>
            <level>2</level>
            <module_config>
                <gpios>
                    <gpio>
                        <name>wiegand_green</name>
                        <no>3</no>
                        <direction>out</direction>
                        <value>false</value>
                    </gpio>
                    <gpio>
                        <name>wiegand_buzzer</name>
                        <no>2</no>
                        <direction>out</direction>
                        <value>false</value>
                    </gpio>
                    <!-- Prepare GPIO for our Wiegand reader -->
                    <gpio>
                        <name>wiegand_data_high</name>
                        <no>1</no>
                        <direction>in</direction>
                    </gpio>
                    <gpio>
                        <name>wiegand_data_low</name>
                        <no>0</no>
                        <direction>in</direction>
                    </gpio>
                </gpios>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
