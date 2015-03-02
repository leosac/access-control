Led Module Documentation {#mod_ledbuzzer_main}
==============================================

[TOC]

Introduction {#mod_ledbuzzer_intro}
===================================

Welcome to the main documentation page for the LedBuzzer module.
This page is about using and configuring the Led module from an end-user point of view. It
also support some buzzer (like the one attached to some reader) because they work in the 
exact same way.

Currently this module is the backend for [FLED](@ref Leosac::Hardware::FLED) and
[FBuzzer](@ref Leosac::Hardware::FBuzzer) object.


Configuration Options {#mod_ledbuzzer_user_config}
==================================================

Below are the configuration options available. Note that this module require
that some FGPIO device be available and configured as it needs them to 
control the Led device.


Options    | Options  | Options                | Description                                            | Mandatory
-----------|----------|------------------------|--------------------------------------------------------|-----------
leds       |          |                        | List of configured Led devices                         | NO
--->       | led      |                        | Configure of one particular led device                 | NO
--->       | --->     | name                   | Chosen name for this led device                        | YES
--->       | --->     | gpio                   | Name of the underlying GPIO used by the Led            | YES
--->       | --->     | default_blink_duration | Time to blink (ms )if no explicit duration is specified| YES
--->       | --->     | default_blink_speed    | Blink speed (in ms) if no speed is specified           | YES


You can use "buzzers" and "buzzer" instead of "leds" and "led", or you can use all four if you want.

Example {#mod_ledbuzzer_example}
--------------------------------

This is a example of led possible configuration for Led module into Leosac.\n
This assume that a `my_gpio_1` and `my_gpio_2` gpios devices are configured.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>LED_BUZZER</name>
            <file>libled.so</file>
            <level>3</level>
            <module_config>
                <leds>
                    <led>
                        <name>my_red_led</name>
                        <gpio>my_gpio_1</gpio>
                        <default_blink_duration>2000</default_blink_duration>
                        <default_blink_speed>100</default_blink_speed>
                    </led>
                    <led>
                        <name>a_nice_yellow_led</name>
                        <gpio>my_gpio_2</gpio>
                        <default_blink_duration>1000</default_blink_duration>
                        <default_blink_speed>75</default_blink_speed>
                    </led>
                </leds>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_ledbuzzer_notes}
----------------------------

If you want to *implement* a module that supports Led device, see [FLED](@ref Leosac::Hardware::FLED) for description
of commands that **shall** be supported by your module.
