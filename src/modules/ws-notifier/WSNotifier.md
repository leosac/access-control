Web Service Notifier {#mod_ws-notifier_main}
=====================================================

@brief A webservice client that will notify webservices of local events.

[TOC]

Introduction {#mod_ws-notifier_intro}
=======================================

This modules listen to configurable authentication sources and forward the
authentication credentials to some configured web-services.

Currently, only auth source event of type `Leosac::Auth::SourceType::SIMPLE_WIEGAND` are
handled, all the other are simply ignored.

The card information is sent in an HTTP POST request.

The POST field is `card_id` and is the card id in decimal.

Configuration Options {#mod_ws-notifier_user_config}
====================================================

Options        | Options  | URL             | Description                                                    | Mandatory
---------------|----------|-----------------|----------------------------------------------------------------|-----------
sources        |          |                 | Multiples message source (wiegand reader)                      | YES
--->           | source   |                 | Name of one reader to watch for event                          | YES
targets        |          |                 | Remote webservice to send information to                       | NO
--->           | target   |                 | Describe a webservice server target.                           | NO
--->           | --->     | url             | A complete URL that will be used as the request's destination. | YES
--->           | --->     | connect_timeout | The timeout (in milliseconds) for the connection phase. See `CURLOPT_CONNECTTIMEOUT_MS` for more information | NO
--->           | --->     | request_timeout | The timeout for the request, in milliseconds. See `CURLOPT_TIMEOUT_MS`. | NO

@note:
The `connect_timeout` and `request_timeout` defaults to 7000 milliseconds.

Example {#mod_ws-notifier_example}
----------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
  <name>WS_NOTIFIER</name>
  <file>libws-notifier.so</file>
  <level>142</level>
  <module_config>
              <sources>
                <source>MY_WIEGAND_1</source>
              </sources>
              <targets>
                <target>
                  <url>http://10.2.3.100/card</url>
                  <connect_timeout>10000</connect_timeout>
                  <request_timeout>10000</request_timeout>
                </target>
              </targets>
  </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_ws-notifier_notes}
==============================

@see This module is implemented in
[this namespace](@ref Leosac::Module::WSNotifier).
