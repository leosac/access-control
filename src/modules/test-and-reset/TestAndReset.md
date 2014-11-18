TestAndReset Module Documentation {#mod_testandreset_main}
==========================================================

[TOC]

Introduction {#mod_testandreset_intro}
======================================

The TestAndReset module allows end user to have visual and/or audio feedback when
presenting a test card.
It also allow the user to reset Leosac configuration to its factory default.

Configuration Options {#mod_testandreset_user_config}
=====================================================

This is a module that watch input device and check for preconfigured reset/restart card.
Options       | Options   | Options    | Description                                            | Mandatory
--------------|-----------|------------|--------------------------------------------------------|-----------
test_buzzer   |           |            | Device (beeper) to control when test card is read      | NO
test_led      |           |            | Device (led) to control when test card is read         | NO
run_on_start  |           |            | Should we beep/blink when the module starts ?          | NO (defaults to true)
devices       |           |            | List of devices to watch for                           | YES
---->         | device    |            | Watch for event sent by this device                    | YES
---->         | ---->     | name       | Name of the device                                     | YES
---->         | --->      | reset_card | Content / number of the card                           | NO
---->         | --->      | test_card  | Id of the card that trigger test led/beep stuff        | NO

Example {#mod_testandreset_example}
-----------------------------------

This is a example of Rpleth possible configuration for Rpleth module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>ResetWatcher</name>
            <file>libtest-and-reset.so</file>
            <level>101</level>
            <beep_on_start>false</beep_on_start>
            <test_buzzer>my_buzzer</test_buzzer>
            <test_led>my_blinking_led</test_led>
            <module_config>
                <devices>
                    <device>
                        <name>MY_WIEGAND_1</name>
                        <reset_card>40:a0:83:80</reset_card>
                    </device>
                </devices>
            </module_config>
        </module> 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
