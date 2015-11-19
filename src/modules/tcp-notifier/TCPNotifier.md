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

The TCP-Notifier module is flexible. It can spawn multiple instance
of notifier.

This allows the module to act as a client to some servers
using some protocol P1, to some other servers using protocol P2 and
be itself a server for clients using protocol P3.

Protocols are built into this module.


Protocol Specifications {#mod_tcp-notifier_spec}
================================================

Protocol 0 {#mod_tcp-notifier_spec0}
------------------------------------

This protocol is a simple as possible.
It consists of fixed-size packet, and there is only 1 packet type.

The size of a packet is 8 bytes: those are the card's serial number.
They are sent in Network Byte Order (big endian). They honor the format: this means
that if the card is read as Wiegand 26, the 16 bits card number will be extracted
from those 26 bits.

In C, the corresponding type would be `uint64_t`.


Configuration Options {#mod_tcp-notifier_config}
====================================================

Options        | Options  | Options  | Description                                                    | Mandatory
---------------|----------|----------|----------------------------------------------------------------|-----------
instance       |          |          | Multiple instance can coexist. Defines one.                    | NO
--->           | sources  |          | Multiples message source (wiegand reader)                      | NO
--->           | --->     | source   | Name of one reader to watch for event                          | YES
--->           | connect  |          | Remote client to connect to.                                   | NO
--->           | --->     | endpoint | Endpoint to connect to.  Can be given multiple time.           | NO
--->           | bind     |          | URLs to bind to.                                               | NO
--->           | --->     | endpoint | Endpoint to bind to.  Can be given multiple time.              | NO
--->           | protocol |          | ID of the protocol to use. See below.                          | YES

@note The endpoint shall have the form `IP:PORT`.

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
