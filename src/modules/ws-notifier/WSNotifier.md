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

Configuration Options {#mod_event_publish_user_config}
======================================================

Options        | Options  | URL             | Description                                                    | Mandatory
---------------|----------|-----------------|----------------------------------------------------------------|-----------
sources        |          |                 | Multiples message source (wiegand reader)                      | YES
--->           | source   |                 | Name of one reader to watch for event                          | YES
targets        |          |                 | Remote webservice to send information to                       | NO
--->           | target   |                 | Describe a webservice server target.                           | NO
--->           | --->     | url             | A complete URL that will be used as the request's destination.
--->           | --->     | connect_timeout | The timeout (in milliseconds) for the connection phase. See CURLOPT_CONNECTTIMEOUT_MS for more information | NO
--->           | --->     | request_timeout | The timeout for the request, in milliseconds. See CURLOPT_TIMEOUT_MS. | NO

@note:
The `connect_timeout` and `request_timeout` defaults to 7000 milliseconds.
