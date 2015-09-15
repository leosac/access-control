TCP Notifier {#mod_tcp-notifier_main}
=====================================================

@brief A module that can notify, over TCP, remote server of local events.

[TOC]

Introduction {#mod_tcp-notifier_intro}
=======================================

This modules listen to configurable authentication sources and forward the
authentication credentials to some configured tcp-server.

Currently, only auth source event of type `Leosac::Auth::SourceType::SIMPLE_WIEGAND` are
handled, all the other are simply ignored.

Protocol Specifications {#mod_tcp-notifier_spec}
================================================

The protocol is a simple as possible.
It consists of fixed-size packet, and there is only 1 packet type.

The size of a packet is 8 bytes: those are the card's serial number.
They are sent in Network Byte Order (big endian).

In C, the corresponding type would be `uint64_t`.


Configuration Options {#mod_tcp-notifier_config}
====================================================

Options        | Options  | URL             | Description                                                    | Mandatory
---------------|----------|-----------------|----------------------------------------------------------------|-----------
sources        |          |                 | Multiples message source (wiegand reader)                      | NO
--->           | source   |                 | Name of one reader to watch for event                          | YES
targets        |          |                 | Remote webservice to send information to                       | NO
--->           | target   |                 | Describe a tcp server target.                           | NO
--->           | --->     | url             | A complete URL that will be used as the request's destination. | YES

@note The URL has the form `IP:PORT`.

Example {#mod_tcp-notifier_example}
----------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
  <name>TCP_NOTIFIER</name>
  <file>libtcp-notifier.so</file>
  <level>142</level>
  <module_config>
              <sources>
                <source>MY_WIEGAND_1</source>
              </sources>
              <targets>
                <target>
                  <url>10.2.3.100:4242</url>
                </target>
              </targets>
  </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_tcp-notifier_notes}
==============================

@see This module is implemented in
[this namespace](@ref Leosac::Module::TCPNotifier).
