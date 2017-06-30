@page page_module_sysfs_gpio Module: SysFS GPIO

SysFsGpio Module Documentation {#mod_sysfsgpio_main}
====================================================

@brief Driving GPIO pin through Linux's sysfs.

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

Options | Options | Options        | Description                                                                             | Mandatory
--------|---------|----------------|-----------------------------------------------------------------------------------------|-----------
aliases |         |                | Define GPIO aliases. This is useful to support multiple platform                        | **YES**
--->    | default |                | Default name resolution for pin. `__NO__` will be replace by the `no` field             | NO 
--->    | PIN_ID  |                | Option name shall be the pin number, **not** textual `PIN_ID`. Value is the identifier for the pin. | NO
export_path |     |                | Absolute path to "export" sysfs file                                                    | **YES**
unexport_path |   |                | Absolute path to "unexport" sysfs file                                                  | **YES**
value_path |      |                | Absolute path to "value" file. `__PLACEHOLDER__` shall act as a placeholder for pin identifier.   | **YES**
edge_path  |      |                | Absolute path to "edge" file. `__PLACEHOLDER__` shall act as a placeholder for pin identifier.    | **YES**
direction_path |  |                | Absolute path to "direction" file. `__PLACEHOLDER__` shall act as a placeholder for pin identifier | **YES**
gpios   |         |                | List of GPIOs pins we configure                                                         | **YES**
--->    | gpio    |                | Configuration informations for one GPIO pin.                                            | **YES**
--->    | --->    | name           | Name of the GPIO pin                                                                    | **YES**
--->    | --->    | no             | Number of the GPIO pin.                                                                 | **YES**
--->    | --->    | direction      | Direction of the pin. This in either `in` or `out`                                      | **YES**
--->    | --->    | interrupt_mode | What interrupt do we care about? See below for details                                  | NO
--->    | --->    | value          | Default value of the PIN. Either `1` or `0`                                             | NO

Path information
----------------
Path configuration allows the user the use the same `sysfsgpio` module on multiple platform even when
the path-to-gpio / naming-convention of GPIO pins varies.
The `__PLACEHOLDER__` placeholder will be replaced by the *identifier* of the pin.
The *identifier* is computed through aliases resolution.

A simple example:
    + Pin id / ping number is `14`.
    + `value_path` = `/sys/class/gpio/__PLACEHOLDER__/value`.
    + `aliases.default` = `gpio__NO__`.
    + The pin *identifier* is resolved as `gpio14` (since `__NO__` from the default alias is replace
      by `14`).
    + `__PLACEHOLDER__` is replaced by the pin identifier: The module will resolve
      the value path of the PIN to `/sys/class/gpio/gpio14/value`
    
@see SysFsGpioConfigTest for more example.

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
                <aliases>
                    <default>gpio__NO__</default>
                </aliases>
                <export_path>/sys/class/gpio/export</export_path>
                <unexport_path>/sys/class/gpio/unexport</unexport_path>
                <edge_path>/sys/class/gpio/__PLACEHOLDER__/edge</edge_path>
                <value_path>/sys/class/gpio/__PLACEHOLDER__/value</value_path>
                <direction_path>/sys/class/gpio/__PLACEHOLDER__/direction</direction_path>                
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
