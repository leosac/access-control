TestAndReset Module Documentation {#mod_testandreset_main}
==========================================================

[TOC]

Introduction {#mod_testandreset_intro}
======================================

The TestAndReset module allows end user to have visual and/or audio feedback when
presenting a test card.
It also allow the user to reset Leosac configuration to its factory default.

@note This module is only able to watch module that sends SIMPLE_WIEGAND as auth-source
 data format.

Configuration Options {#mod_testandreset_user_config}
=====================================================

This is a module that watch input device and check for preconfigured reset/restart card.
Options       | Options   | Options    | Description                                                    | Mandatory
--------------|-----------|------------|----------------------------------------------------------------|-----------
test_buzzer   |           |            | Device (beeper) to control when test card is read              | NO
test_led      |           |            | Device (led) to control when test card is read                 | NO
run_on_start  |           |            | Should we beep/blink when the module starts ?                  | NO (defaults to true)
promisc       |           |            | Listen to all event to watch for reset card.                   | NO (defaults to false)
reset_card    |           |            | Wiegand ID of reset card. Only useful if promisc is `true`.    | YES if promisc is `true`. 
devices       |           |            | List of devices to watch for                                   | NO
--->          | device    |            | Watch for event sent by this device                            | YES
--->          | --->      | name       | Name of the device                                             | YES
--->          | --->      | reset_card | Content / number of the card                                   | NO
--->          | --->      | test_card  | Id of the card that trigger test led/beep stuff                | NO

About `promisc` mode: It will listen to everything on the message bus to try and detect the reset card.
This comes at a little performance cost because we watch way more message than needed. However, this
has the advantage of working without specifying a source device, which may be helpful when
synchronizing configuration.

Example {#mod_testandreset_example}
-----------------------------------

This is a example of Rpleth possible configuration for Rpleth module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <module>
        <name>TEST_AND_RESET</name>
        <file>libtest-and-reset.so</file>
        <level>101</level>
        <module_config>
            <run_on_start>false</run_on_start>
            <test_buzzer>my_buzzer</test_buzzer>
            <test_led>my_blinking_led</test_led>
            <devices>
                <device>
                    <name>MY_WIEGAND_1</name>
                    <reset_card>40:a0:83:80</reset_card>
                </device>
            </devices>
        </module_config>
    </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
