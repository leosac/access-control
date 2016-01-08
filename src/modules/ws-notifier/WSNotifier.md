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

The POST field is `card_id` and represents the card id, in decimal.

Configuration Options {#mod_ws-notifier_user_config}
====================================================

Options        | Options  | Options         | Description                                                    | Mandatory
---------------|----------|-----------------|----------------------------------------------------------------|-----------
want_ssl       |          |                 | Do we enable the SSL engine ?                                  | NO (defaults to `true`)
sources        |          |                 | Multiples message source (wiegand reader)                      | NO
--->           | source   |                 | Name of one reader to watch for event                          | YES
targets        |          |                 | Remote webservice to send information to                       | NO
--->           | target   |                 | Describe a webservice server target.                           | NO
--->           | --->     | url             | A complete URL that will be used as the request's destination. | YES
--->           | --->     | connect_timeout | The timeout (in milliseconds) for the connection phase. See `CURLOPT_CONNECTTIMEOUT_MS` for more information | NO
--->           | --->     | request_timeout | The timeout for the request, in milliseconds. See `CURLOPT_TIMEOUT_MS`. | NO
--->           | ---->    | ca_file         | Path to a PEM encoded CA file used to validate certificate. | NO
--->           | --->     | verify_host     | If SSL is enabled, do we verify the host name in the SSL certificate ? | NO (defaults to `true`)   
--->           | --->     | verify_peer     | If SSL is enabled, do we verify the SSL certificate ? | NO (defaults to `true`)   

@note
The `connect_timeout` and `request_timeout` defaults to 7000 milliseconds.

@note
`want_ssl` defaults to true, meaning you'll be able to contact webservice over HTTPS. 
You can prevent the module from loading the SSL engine by setting this to `false`.

Example {#mod_ws-notifier_example}
----------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
  <name>WS_NOTIFIER</name>
  <file>libws-notifier.so</file>
  <level>142</level>
  <module_config>
              <want_ssl>true</want_ssl>
              <sources>
                <source>MY_WIEGAND_1</source>
              </sources>
              <targets>
                <target>
                  <url>http://10.2.3.100/card</url>
                  <connect_timeout>10000</connect_timeout>
                  <request_timeout>10000</request_timeout>
                  <verify_host>true</verify_host>
                  <verify_peer>true</verify_peer>
                  <ca_file>/opt/server.pem</ca_file>
                </target>
              </targets>
  </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_ws-notifier_notes}
==============================

@see This module is implemented in
[this namespace](@ref Leosac::Module::WSNotifier).
