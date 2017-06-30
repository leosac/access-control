@page page_module_websock_api Module: WebSocket API

WebSocket API {#mod_websock-api_main}
=====================================

@brief Expose a web-socket based API to Leosac.

[TOC]

Introduction {#mod_websock-api_intro}
=======================================

OUTDATED ! WILL NEED REWRITE ONCE THE API STABILIZES.

The module expose Leosac's internals and various information
through a websocket API.
This API is intended to be mostly consumed by a web interface.

We'll start by describing the packet format, and then enumerate
the available API call.


Packet Format {#mod_websock-api_format}
=======================================

From clients to Leosac {#mod_websock-api_format_srv-to-client}
--------------------------------------------------------------

The message format sent to Leosac by clients is simple.

Label         | Description                                                 
--------------|-----------------------------------------------------------------
type          | A required string describing the API endpoint targeted by the message.
uuid          | A UUID string that uniquely identify the message.
content       | Message specific content.


### Authentication attempt {#mod_websock-api_format_client-to-srv_ex1}

~~~~~~~~~~~~~~~~~~~~~~.json
{
   "type": "create_auth_token",
   "uuid": "9c33e119-1d6f-4f13-94e2-d9b670f63874",
   "content":
       {
          "username": "toto",
          "password": "titi"
       }
}
~~~~~~~~~~~~~~~~~~~~~~

From Leosac to client {#mod_websock-api_format_srv-to-client}
-------------------------------------------------------------

This section describes the format of Websocket messages sent by Leosac.
They use a well defined base structure, of which a `content` field is used
for message specific data.

Label         | Description                                                 
--------------|-----------------------------------------------------------------
status_code   | An integer representing the status code of request processing.
status_string | An *optional* string explaining the `status`.
type          | A required string describing the API endpoint attached to the message.
uuid          | A UUID string that uniquely identify the message.
content       | Message specific content.

**Note**:
    1. `status` indicates whether or not processing the request went well. 
      As an example, even a failed authentication would have 
      a global status code of Leosac::APIStatusCode::SUCCESS. The authentication failure
      would be specified in the `content` field of the packet.
    2. If the message is a response to a request, it will copy the request's UUID.
      Otherwise a new UUID will be used.
    3. The `type` is useful only for message that are not sent in response to a request.
      However, it is still required. When responding to a request, the 
    3. See each API call for the content they return.

For `status` code value, see Leosac::APIStatusCode.

Below are a few examples in a few different situations.

### Successful authentication attempt {#mod_websock-api_format_srv-to-client_ex1}

~~~~~~~~~~~~.json
{
    "status_code": 0, // SUCCESS
    "status_string": "",
    "uuid": "6c22846d-a70e-4f10-8bc2-3ecdac7a2344",
    "type": "create_auth_token",
    "content":
        {
           "status": 0,
           "user_id": 42,
           "token": "95a73b39-533d-46df-8892-2295e08896a5"
        }
}
~~~~~~~~~~~~

### Failed authentication attempt {#mod_websock-api_format_srv-to-client_ex2}

In this case, the user failed to authenticate because he sent invalid
credential.

~~~~~~~~~~~~.json
{
    "status_code": 0, // SUCCESS
    "status_string": "",
    "uuid": "58292dce-a0a8-403b-9893-36142f819de3", 
    "type": "create_auth_token",
    "content":
        {
           "status": -1 // create_auth_token specific status code.
        }
}
~~~~~~~~~~~~

### Failed authentication attempt 2 {#mod_websock-api_format_srv-to-client_ex3}

In this case, the database crashed when processing the request. The credentials
validity is unknown.

~~~~~~~~~~~~.json
{
    "status_code": 1, // GENERAL_FAILURE
    "status_string": "Database error",
    "type": "create_auth_token",
    "uuid": "a4d12394-f4b3-4bf3-a51e-4b8179ac13b5,
    "content":
        {
          // No content
        }
}
~~~~~~~~~~~~

### Failed authentication attempt 3 {#mod_websock-api_format_srv-to-client_ex4}

In this case, the user was already authenticated. The request didn't
made it to the create_auth_token() call.

~~~~~~~~~~~~.json
{
    "status_code": 2, // PERMISSION_DENIED
    "status_string": "Already logged in",
    "type": "create_auth_token",
    "uuid": "1c30418a-e349-4d71-9eee-908b84e0cf57"
    "content":
        {
          // No content
        }
}
~~~~~~~~~~~~

API Documentation {#mod_websock-api_doc}
==========================================

This parts describe the API provided by the module.

The websocket API is asynchronous. Requests are identified by a UUID generated
by the client, and the response will re-use this UUID to indicate a response
that match said request.

Opportunistic messages from server (aka not a response to a request) will
also use an UUID, this time generated by the server.


General fully public API {#mod_websock-api_public}
--------------------------------------------------

This parts of the API is fully public, meaning that it doesn't require
authentication in order to issue requests.

   + [get_leosac_version](@ref Leosac::Module::WebSockAPI::API::get_leosac_version)
     Retrieve the version of the running server.


Authentication {#mod_websock-api_authentication}
------------------------------------------------

To do pretty much anything against Leosac you need to authenticate.

   + [create_auth_token](@ref Leosac::Module::WebSockAPI::API::create_auth_token):
     Create an authentication token from user/password credential.
   + [authenticate_with_token](@ref Leosac::Module::WebSockAPI::API::authenticate_with_token):
     Authenticate with a previously generated token.
   + [logout](@ref Leosac::Module::WebSockAPI::API::logout):
     Cancel authentication status and destroy token.
   

General API {#mod_websock-api_general}
---------------------------------------

These API method are available to authenticated clients.

   + [system_overview](@ref Leosac::Module::WebSockAPI::API::system_overview):
     Retrieve general information about the system.
   + [get_logs](@ref Leosac::Module::WebSockAPI::API::get_logs):
     Retrieve logs generated by the Leosac server.
   + [user_get](@ref Leosac::Module::WebSockAPI::API::user_get):
     Retrieve information regarding a specific user.
