@page page_module_wiegand Module: Wiegand

Wiegand Module Documentation {#mod_wiegand_main}
================================================

[TOC]

@brief Explains how to configure the module to work with various Wiegand reader.

Introduction {#mod_wiegand_intro}
=================================

This module provides support for Wiegand devices. Simply put, it listens to GPIO event,
gather them, and generate Wiegand events. It supports multiple readers.

As this is an module that provide an authentication source (by reading card ids or PIN code, or both), it shall
conforms to [those specifications](@ref auth_specc) regarding message passing.

There are multiple mode supported by the wiegand module. They allows the module to read from differently configured
hardware, and allows you to chose what kind of credentials the module shall generate.

Configuration Options {#mod_wiegand_user_config}
================================================

Options      | Options  | Options     | Description                                                | Mandatory
-------------|----------|-------------|------------------------------------------------------------|-----------------------
use_database |          |             | If true, use the database for config. Ignore other options | NO (defaults to false)
readers      |          |             | Lists of all configured readers                            | YES
--->         | reader   |             | Configuration of 1 wiegand reader                          | YES
--->         | --->     | name        | device name                                                | YES
--->         | --->     | high        | name of the input GPIO that sends "high" data              | YES
--->         | --->     | low         | name of the input GPIO that sends "low" data               | YES
--->         | --->     | green_led   | name of the green led device attached to the reader        | NO
--->         | --->     | buzzer      | name of the buzzer device attached to the reader           | NO
--->         | --->     | mode        | Which mode the reader is using (see below)                 | NO (defaults to `SIMPLE_WIEGAND`)
--->         | --->     | pin_timeout | Timeout when reading a PIN code.                           | NO (defaults to 2500ms)
--->         | --->     | pin_key_end | Which key is used to signal the end of a PIN code          | NO (defaults to '#')
--->         | --->     | nowait      | Don't wait for pin code after card read                    | NO (defaults to 0)

**Note**: `high`, `low`, `green_led` and `buzzer` must be name of GPIO object: either defined using
the sysfsgpio or pifacedigital module.

There are multiples `mode` available for a reader:
1. `SIMPLE_WIEGAND` is for simply reading a wiegand card.
2. `WIEGAND_PIN_4BITS` for reading a PIN code, when the reader send 4 bits per key pressed.
3. `WIEGAND_PIN_8BITS` for reading a PIN code, when the reader send 8 bits per key pressed.
4. `WIEGAND_PIN_BUFFERED` for reading a PIN code when the reader buffers up to 5key and send them in a 26 bits wiegand frame.
  Note that in this mode, the `pin_key_end` and `pin_timeout` parameter have no effect.
5. `WIEGAND_CARD_PIN_4BITS` reads a card number followed by a PIN code read in 4bits mode.
6. `WIEGAND_CARD_PIN_8BITS` reads a card number followed by a PIN code read in 8bits mode.
7. `WIEGAND_CARD_PIN_BUFFERED` reads a card number followed by a PIN code read in buffered mode.
8. `AUTODETECT` allows the module to read and create different type of credentials. It can read a pin code or a card number, or both.
The pin code can be read if the reader is either in 4 bits or 8 bits mode. If you are doing card-only authentication,
note that you will notice a delay of ~2 second, unless nowait is set to true. This delay is here to let the user have a chance to enter his PIN code.

@warning The `AUTODETECT` mode is not compatible with the hardware Wiegand-Buffered mode.

You can see [this](@ref auth_data_type) for more the message passing specific specification
about those modes. Note that multiple mode share a common message implementation.

Additional informations:
+ `pin_timeout` is the number of milliseconds (defaults to 2000) of inactivity on the keypad before
considering the PIN code complete and sending it for processing. It's also the delay you have to enter your PIN after
swiping your card.
+ `pin_key_end` is the key to press to signal the end of the pin (default to '#'). This key wont be appended to the PIN code.
+ You can either type your PIN and wait, and type your PIN and the `pin_key_end`.


Example {#mod_wiegand_example}
------------------------------

This is a example of configuration for the Wiegand module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>WIEGAND_READER</name>
    <file>libwiegand.so</file>
    <level>5</level>
    <module_config>
        <readers>
            <reader>
                <name>MY_WIEGAND_1</name>
                <high>wiegand_data_high</high>
                <low>wiegand_data_low</low>
                <green_led>WIEGAND1_GREEN_LED</green_led>
                <buzzer>WIEGAND1_BUZZER</buzzer>
            </reader>
        </readers>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            
Notes {#mod_wiegand_notes}
==========================

Technical implementations details can be found [here](@ref Leosac::Module::Wiegand).
