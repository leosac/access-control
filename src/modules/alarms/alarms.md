@page page_module_alarms Module: Alarms

Alarms Module Documentation {#mod_alarms_main}
================================================

@brief Module that reacts to alarm event.

[TOC]

Introduction {#mod_alarms_intro}
=================================

The Alarms module track alarms event.

Configuration Options {#mod_alarms_user_config}
================================================

Options      | Options   | Options      | Description                                                       | Mandatory
-------------|-----------|--------------|-------------------------------------------------------------------|-----------
use_database |           |              | If true, use database-backed configuration.                       | NO (defaults to false)
alarms       |           |              | Multiples (unrelated) alarms instance are allowed                 | YES
--->         | alarm     |              | Configuration information for 1 instance                          | YES
--->         | --->      | name         | Name of this alarm instance                                       | YES
--->         | --->      | severity     | The alarm's severity                                              | NO (defaults to NORMAL)
--->         | --->      | gpio         | The GPIO to toggle when an alarm is raised                        | NO


Example {#mod_alarms_example}
-----------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>ALARMS</name>
    <file>libalarms.so</file>
    <level>5</level>
    <module_config>
        <alarms>
            <alarm>
                <name>ALARM_1</name>
                <!-- 2 equals NORMAL severity -->
                <severity>2</severity>
            </alarm>
        </alarms>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
