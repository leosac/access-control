SysFsGpio Module Documentation {#mod_sysfsgpio_main}
====================================================

[TOC]

Introduction {#mod_sysfsgpio_intro}
===================================

Welcome to the main documentation page for the SysFsGpio module.
This page is about SysFsGpio from an end-user point of view. \n
It describes how to configure the SysFsGpio module for Leosac.

This module provide support for FGPIO device by supporting the
Linux Kernel SysFs interface. It allows the application to deals with
GPIO pin that are in fact controlled through sysfs.

Configuration Options {#mod_sysfsgpio_user_config}
==================================================

Below are the configuration options available.

Options | Options | Options        | Description                                            | Mandatory
--------|---------|----------------|--------------------------------------------------------|-----------
gpios   |         |                | List of GPIOs pins we configure                        | YES
----->  | gpio    |                | Configuration informations for one GPIO pin.           | YES
----->  | ----->  | name           | Name of the GPIO pin                                   | YES
----->  | ----->  | no             | Number of the GPIO pin.                                | YES
----->  | ----->  | direction      | Direction of the pin. This in either `in` or `out`     | YES
----->  | ----->  | interrupt_mode | What interrupt do we care about? See below for details | NO
----->  | ----->  | value          | Default value of the PIN. Either `1` or `0`            | NO

Interrupt Mode
--------------
The configuration option `interrupt_mode` can take 4 values:
     + `Falling`
     + `Rising`
     + `Both`
     + `None`. This is the default.
This parameter is ignored for output pin.

Default Value
-------------
The default value (ignored for input pin) is set for the pin when the module
starts. It is also restored when the module stops.


Example {#mod_sysfsgpio_example}
--------------------------------

This is a example of SysFsGpio possible configuration for SysFsGpio module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>SYSFS-GPIO</name>
            <file>libsysfsgpio.so</file>
            <level>2</level>
            <module_config>
                <gpios>
                    <gpio>
                        <name>wiegand_data_high</name>
                        <no>15</no>
                        <direction>in</direction>
                        <interrupt_mode>falling</interrupt_mode>
                    </gpio>
                    <gpio>
                        <name>wiegand_data_low</name>
                        <no>14</no>
                        <direction>in</direction>
                        <interrupt_mode>falling</interrupt_mode>
                    </gpio>
                    <gpio>
                        <name>a_random_gpio</name>
                        <no>3</no>
                        <direction>out</direction>
                        <value>0</value>
                    </gpio>
                </gpios>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            
Notes {#mod_sysfsgpio_notes}
============================

@see For technical description of the module, see the
[main class for the module](@ref Leosac::Module::SysFsGpio::SysFsGpioModule) .