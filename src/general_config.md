General Configuration Information {#general_config_main}
========================================================

[TOC]

Introduction {#general_config_intro}
====================================

Even though Leosac is heavily based on modules, they are general
configuration options. Those are defined in the main configuration file,
outside of the `<modules>` and `</modules>` tags.

Here is an overview of what we call "general configuration":
  + Logger configuration
  + Network configuration
  + Remote control configuration.
  
Logger Configuration {#general_config_logger}
=============================================

Leosac has a builtin logger for information and / or debugging purposes. It can write to syslog.
It is not recommended to log DEBUG message to syslog.


Configuration Options {#logger_user_config}
-------------------------------------------

Default configuration will log everything to `stdout`. Entry with 
a level >= `WARNING` will be written to syslog.

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



Network Configuration {#general_config_network}
================================================

Leosac can provide an abstraction to network configuration.
You can configure your IP address, netmask and gateway in Leosac's configuration file.

This allows the Remote Control API (or Rpleth API) to change the network configuration
of the Leosac unit.

However, if you do not plan on changing your IP address or see no use for this feature,
it is recommended to use Linux tools to configure your network instead (ie, write to `/etc/network/interface/`).

Example {#network_example}
--------------------------
Network configuration takes place in the `<kernel>` tag. It is **not** configured
as a module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <network>
        <enabled>false</enabled>
    </network>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An other example with enabled network management.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <network>
        <enabled>true</enabled>
        <interface>eth0</interface>
        <dhcp>true</dhcp>
        <netmask>255.255.255.0</netmask>
        <default_ip>192.168.0.18</default_ip>
        <gateway>192.168.0.1</gateway>
    </network>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Remote Control {#remote_control}
================================

The Remote Control API is more complicated than logger or network configuration,
and require a more in depth description. You can find it [here](@ref remote_control_main).