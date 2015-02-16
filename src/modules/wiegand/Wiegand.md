Wiegand Module Documentation {#mod_wiegand_main}
================================================

[TOC]

Introduction {#mod_wiegand_intro}
=================================

This module provides support for Wiegand devices. Simply put, it listens to GPIO event,
gather them, and generate Wiegand events. It supports multiple readers.

As this is an module that provide an authentication source (by reading card ids or PIN code, or both), it shall
conforms to [those specifications](@ref auth_specc) regarding message passing.

There are multiple mode supported by the wiegand module: This mode are known as Auth Source type 
in the application. Each mode may have additional configuration option that can be specified 
in the `reader` configuration.

Configuration Options {#mod_wiegand_user_config}
================================================

Options    | Options  | Options     | Description                                            | Mandatory
-----------|----------|-------------|--------------------------------------------------------|-----------
readers    |          |             | Lists of all configured readers                        | YES
---->      | reader   |             | Configuration of 1 wiegand reader                      | YES
---->      | ---->    | name        | device name                                            | YES
---->      | ---->    | high        | name of the input GPIO that sends "high" data          | YES
---->      | ---->    | low         | name of the input GPIO that sends "low" data           | YES
---->      | ---->    | green_led   | name of the green led device attached to the reader    | NO
---->      | ---->    | buzzer      | name of the buzzer device attached to the reader       | NO
---->      | ---->    | mode        | Which mode the reader is using (see below)             | NO (defaults to SIMPLE_WIEGAND)
---->      | ---->    | pin_timeout | Timeout when reading a PIN code.                       | NO (only for WIEGAND_PIN mode)
---->      | ---->    | pin_key_end | Which key is used to signal the end of a PIN code      | NO (only for WIEGAND_PIN mode)

**Note**: `high`, `low`, `green_led` and `buzzer` must be name of GPIO object: either defined using
the sysfsgpio or pifacedigital module.

There are multiples `mode` available for a reader:
1. `SIMPLE_WIEGAND` is for simply reading a wiegand card.
2. `WIEGAND_PIN_4BITS` for reading a PIN code, when the reader send 4 bits per key pressed.
3. `WIEGAND_PIN_8BITS` for reading a PIN code, when the reader send 8 bits per key pressed.

You can see [this](@ref auth_data_type) for more the message passing specific specification
about those modes. Note that multiple mode share a common message implementation.

Additional informations:
+ `pin_timeout` is the number of milliseconds (defaults to 2000) of inactivity on the keypad before
considering the PIN code complete and sending it for processing.
+ `pin_key_end` is the key to press to signal the end of the pin (default to '#'). This key wont be appended to the PIN code.
+ You can either type your PIN and wait, and type your PIN and the `pin_key_end`.


Example {#mod_wiegand_example}
------------------------------

This is a example of Rpleth possible configuration for Rpleth module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>WIEGAND</name>
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
