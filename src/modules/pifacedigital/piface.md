@page page_module_piface Module: Piface Digital GPIO

PifaceDigital Module Documentation {#mod_piface_main}
=====================================================

[TOC]

@brief Explain how to use the Piface board as a GPIO backend.

Introduction {#mod_piface_intro}
=================================

Documentation for the PifaceDigital module.
This provide support for using the PifaceDigital device as a GPIO controller.
This can be very handy because it eases wiring.

This module requires that the Linux Kernel Module SPI be enabled.
You can do this through `raspi-config` on Raspbian.

Configuration Options {#mod_piface_user_config}
================================================

See below for various configuration options.


Options      | Options   | Description                                            | Mandatory
-------------|-----------|--------------------------------------------------------|-----------
use_database |           | If true, use database-backed configuration.            | NO (defaults to false)
gpios        |           | GPIOS definitions                                      | YES
--->         | name      | A name for the GPIO device                             | YES
--->         | no        | The GPIO number of the piface. Range from 0 to 7       | YES
--->         | direction | Direction of the PIN. in or out                        | YES
--->         | value     | Only for out PIN. The default value of the PIN         | YES for output pin

Notes:
+ If `use_database` is true, the module will expose its configuration API over
  websocket (provided that a Websocket module is available). All other XML configuration
  option are ignored.
+ `value` is a boolean. It's only for output GPIO and represents the default value.


Database Configuration Notes
----------------------------

It is not possible to mix both database configuration and XML configuration.
If the XML configuration set `use_database` to `true`, then all other XML
configuration values are ignored.

When configuration is updated (create/update/delete gpio objects) is not
loaded by the module. Restarting Leosac is required to apply new configuration.

XML Config Example {#mod_piface_example}
------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>PIFACEDIGITAL_GPIO</name>
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
