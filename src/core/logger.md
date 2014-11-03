Logger Documentation {#logger_main}
===================================

[TOC]

Introduction {#logger_intro}
============================

This page explain how to configure the logging facility Leosac provides.


Configuration Options {#logger_user_config}
===========================================

Below are the configuration options available. Note that this module require
that some FGPIO device be available and configured as it needs them to 
control the logger device.


Options       | Options  | Options | Description                                      | Mandatory
--------------|----------|---------|--------------------------------------------------|-----------
enable_syslog |          |         | Enable logging to syslog                         | NO (default to `true`)
min_syslog    |          |         | Minimal log entry level to write to syslog       | NO (default to `WARNING`)

Example {#logger_example}
-------------------------

Logger configuration takes place in the `<kernel>` tag. It is **not** configured
as a module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <log>
        <enable_syslog>true</enable_syslog>
        <min_syslog>INFO</min_syslog>
    </log>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~