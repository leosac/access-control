Instrumentation Module Documentation {#mod_instrumentation_main}
================================================================

[TOC]

Introduction {#mod_instrumentation_intro}
=========================================

This modules help testing. It expose fake GPIO ping that are controllable
through IPC, thus allowing some kind of blackbox testing against Leosac.

List of remote command:
+ "GPIO" "my_gpio" "ON": 3 Frames, turn the GPIO `ON`.
+ "GPIO" "my_gpio" "OFF": 3 Frames, turn the GPIO `OFF`.
+ "GPIO" "my_gpio" "INT": 3 Frames, emulate GPIO interrupt.

Configuration Options {#mod_instrumentation_user_config}
========================================================

Options           | Options  | Options     | Description                                                 | Mandatory
------------------|----------|-------------|-------------------------------------------------------------|-----------
ipc_endpoint      |          |             | Path to the IPC endpoint. Must be able to create this file. | YES


Notes:
  + Leosac will prefix `ipc://` to the `ipc_endpoint` and will bind to this.

Example {#mod_instrumentation_example}
--------------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>Instrumentation</name>
            <file>libinstrumentation.so</file>
            <level>105</level>
            <module_config>
                <ipc_endpoint>/tmp/leosac-ipc</ipc_endpoint>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
