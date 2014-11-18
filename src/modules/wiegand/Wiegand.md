Wiegand Module Documentation {#mod_wiegand_main}
================================================

[TOC]

Introduction {#mod_wiegand_intro}
=================================

This module provides support for Wiegand devices. Simply put, it listens to GPIO event,
gather them, and generate Wiegand events. It supports multiple readers.

As this is an module that provide an authentication source (by reading card ids), it shall
conforms to [those specifications](@ref auth_specc) regarding message passing.

Configuration Options {#mod_wiegand_user_config}
================================================

Options    | Options  | Options    | Description                                            | Mandatory
-----------|----------|------------|--------------------------------------------------------|-----------
readers    |          |            | Lists of all configured readers                        | YES
---->      | reader   |            | Configuration of 1 wiegand reader                      | YES
---->      | ---->    | name       | device name                                            | YES
---->      | ---->    | high       | name of the input GPIO that sends "high" data          | YES
---->      | ---->    | low        | name of the input GPIO that sends "low" data           | YES
---->      | ---->    | green_led  | name of the green led device attached to the reader    | NO
---->      | ---->    | buzzer     | name of the buzzer device attached to the reader       | NO

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
