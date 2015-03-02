Monitor Module Documentation {#mod_monitor_main}
============================================

[TOC]

Introduction {#mod_monitor_intro}
=================================

Documentation of the (activity) monitor module.

Configuration Options {#mod_monitor_user_config}
================================================

Below are the configuration options available. Note that this module require
that some FGPIO device be available and configured as it needs them to 
control the Led device.


Options    | Options  | Description                                            | Mandatory
-----------|----------|--------------------------------------------------------|-----------
file-bus   |          | Where we write event from the application bus          | NO
verbose    |          | Be verbose and write to stdout everything we log       | NO
ping       |          | Configure network testing                              | NO
--->       | ip       | An ip address to ping to check network connectivity    | YES
--->       | led      | Name of led that represents status of network          | YES
reader     |          | Feedback for reader activity                           | NO
--->       | name     | Name of the reader object to watch                     | YES
--->       | led      | Led to turn ON when we detect reader activity          | YES
system_ok  |          | A led to turn ON when the system is ready              | NO

Notes:
+ `file-bus`: If not set (or empty) we ignore the system bus.
+ `verbose`: default to false.
+ `system_ok`: this led should have `false` has its default value, otherwise it doesn't make sense as it will
stay on, always.


Example {#mod_monitor_example}
------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <module>
        <name>MONITOR</name>
        <file>libmonitor.so</file>
        <level>1</level>
        <module_config>
            <file-bus>MY_LOGS.txt</file-bus>
            <verbose>false</verbose>
        </module_config>
    </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
