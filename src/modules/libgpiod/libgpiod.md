@page page_module_libgpiod Module: Libgpiod

Libgpiod Module Documentation {#mod_libgpiod_main}
====================================================

@brief Driving GPIO pin through Linux's Character Device GPIO and libgpiod.

[TOC]

Introduction {#mod_libgpiod_intro}
===================================

Welcome to the main documentation page for the Libgpiod module.
This page is about Libgpiod from an end-user point of view. \n
It describes how to configure the Libgpiod module for Leosac.

This module provide support for FGPIO device by supporting the
Linux Kernel Libgpiod interface. It allows the application to deals with
GPIO pin that are in fact controlled through libgpiod.

Configuration Options {#mod_libgpiod_user_config}
==================================================

Below are the configuration options available.

Options | Options | Options        | Description                                                                             | Mandatory
---------|---------|----------------|-----------------------------------------------------------------------------------------|-----------
consumer |         |                | The GPIOs consumer name (default: leosac)                                               | NO
gpios    |         |                | List of GPIOs pins we configure                                                         | **YES**
--->     | gpio    |                | Configuration informations for one GPIO pin.                                            | **YES**
--->     | --->    | name           | Name of the GPIO pin                                                                    | **YES**
--->     | --->    | device         | GPIO chip device.                                                                       | **YES**
--->     | --->    | offset         | Offset of the GPIO pin.                                                                 | **YES**
--->     | --->    | direction      | Direction of the pin. This in either `in` or `out`                                      | **YES**
--->     | --->    | interrupt_mode | What interrupt do we care about? See below for details                                  | NO
--->     | --->    | value          | Default value of the PIN. Either `1` or `0`                                             | NO

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


Example {#mod_libgpiod_example}
--------------------------------

This is a example of Libgpiod possible configuration for Libgpiod module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>LIBGPIOD</name>
            <file>liblibgpiod.so</file>
            <level>2</level>
            <module_config>
                <consumer>leosac</consumer>                
                <gpios>
                    <gpio>
                        <name>wiegand_data_high</name>
                        <device>gpiochip0</device>
                        <offset>15</offset>
                        <direction>in</direction>
                        <interrupt_mode>falling</interrupt_mode>
                    </gpio>
                    <gpio>
                        <name>wiegand_data_low</name>
                        <device>gpiochip0</device>
                        <offset>14</offset>
                        <direction>in</direction>
                        <interrupt_mode>falling</interrupt_mode>
                    </gpio>
                    <gpio>
                        <name>a_random_gpio</name>
                        <device>gpiochip0</device>
                        <offset>3</offset>
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
