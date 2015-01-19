Network Documentation {#network_main}
=====================================

[TOC]

Introduction {#network_intro}
=============================

This page explain how to configure the device's network configuration through
Leosac facility.
If you don't plan on changing the network configuration, it is recommended to use
Linux tools to configure it instead (ie, write to `/etc/network/interface/`)


Configuration Options {#network_user_config}
============================================

Refer to example.
Default configuration disable network management for leosac.

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
