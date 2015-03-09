Authentication Documentation {#auth_main}
============================================

[TOC]

[AccessProfile]: @ref Leosac::Auth::IAccessProfile "Access Profile"
[Cards]: @ref Leosac::Auth::ICard "Card"
[Users]: @ref Leosac::Auth::IUser "User"

@brief Explain how the authentication and authorization subsystem works.

Introduction {#auth_intro}
=============================

In an access control system, the authentication & authorization subsystem is complex.
This page aims to explain how it works in Leosac.

The task of reading credentials, validating them and taking action (opening doors) is split
over multiple module. This gives us more flexibility and helps keeping the code clear and relatively simple.

Below is a diagram that illustrate how thing works:
![Auth overview diagram](../overview_auth.png)


Two kinds of modules
====================

We break down the authentication & authorization code in multiple parts.
There are two kinds of modules:
    1. Authentication Source module (eg [Wiegand](@ref mod_wiegand_main))
    2. Authentication Backend module (eg [Files](@ref mod_auth_file_main))

Authentication Source
---------------------

An **authentication source** module provides user-data to the system, for example
a card id or a pin code. Theses modules are first in the chain of event that lead
to an access control.

An authentication **shall** have at least one property: a name.
This name will be used throughout the application to determine what device (or authentication
source) triggered the events.

TODO:
Also we need something to identify the [type](ref auth_data_type) of the data. (pin vs pin/card id vs card id vs ...)

Authentication Backend
----------------------

An **authentication backend** module gather event from **authentication source**
and check their own backend to build a profile (raw files, database, remote webservices, ...)

The profile is built by checking group-membership, credentials mapping, etc.
The newly built profile is then queried to determine whether or not the user should be granted access.

In order for this to work properly, we need a decent message passing specifications.

Authentication data type {#auth_data_type}
------------------------------------------

Data type information helps authentication backends understand messages from authentication sources.
It is an enumeration value, see [AuthSourceType](@ref Leosac::Auth::SourceType).

Currently we are lucky, we only define a few.

1. **SIMPLE_WIEGAND**: This data type requires 2 frames:
  1. An hexadecimal *string* that contain the card id that was read, for example: "40:a0:83:80"
  2. An *integer* that holds the number of bits in the card id. 
2. **WIEGAND_PIN**: Configuration mode used when we use the reader only for PIN code.
  1. A *string* that contains the PIN code. Its easier to use string, because we don't know the length
    of the PIN code, and it may vary.
3. **WIEGAND_CARD_PIN**: This credential mode holds both a wiegand card number along with a PIN code.
  1. An hexadecimal *string* that contain the card id that was read, for example: "40:a0:83:80"
  2. An *integer* that holds the number of bits in the card id.
  3. A *string* that contains the PIN code.

Message Passing Specifications {#auth_specc}
============================================

Understand the [application message bus](@ref MessageBus) first.

First, a few assumptions. Authentication data from auth source may vary widely.
Consider a simple pin code versus a wiegand card id. The wiegand card must holds 2 data: the value, and the
number of meaningful bits.
A wiegand reader can also handle PIN code + card id, augmenting complexity of the auth data.

Auth backend must be able to distinguish between those to handles them properly.

### Authentication Source

Workflow for an **authentication source** module:
 1. Having enough meaningful data that represents an authentication source (card id, pin code, fingerprint, ...)
    the module shall publish a message on the message bus the let others know.
 2. The message shall be multiparts.
    1. Its first frame must be the auth source's name prefixed by `S_`.
    2. The second frame shall be the type of data. See [this](@ref auth_data_type).    
    3. Optional (but likely presents) frames that contains different data based on the [auth data type](@ref auth_data_type)


As an example, consider a message from the [wiegand module](@ref mod_wiegand_main), named `MY_WIEGAND_1`:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------------------------
1        | "MY_WIEGAND_1"                                | `string`
2        | `Leosac::Auth::SourceType::SIMPLE_WIEGAND`    | [AuthSourceType](@ref Leosac::Auth::SourceType) (`uint8_t`)
3        | "12:af:cd:21"                                 | `string`
4        | 26                                            | `int` 


A second example for the `WIEGAND_PIN` auth source mode:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------------------------
1        | "MY_WIEGAND_1"                                | `string`
2        | `Leosac::Auth::SourceType::WIEGAND_PIN`       | [AuthSourceType](@ref Leosac::Auth::SourceType) (`uint8_t`)
3        | "1234591"                                     | `string`


### Authentication Backend

Workflow for an **authentication backend** module:
 1. Receive a message from an **auth source**.
 2. Process the message, and decide whether access shall be granted or not.
 3. Send a multiparts response message.
   1. The first frame MUST be `S_{AUTH_INSTANCE_NAME}`.
   2. The second frame shall be an [AccessStatus](@ref Leosac::Auth::AccessStatus).

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------------------------
1        | "S_MY_AUTH_SOURCE_1"                          | `string`
2        | `Leosac::Auth::AccessStatus::Granted`         | [AccessStatus](@ref Leosac::Auth::AccessStatus) (`uint8_t`)

@namespace Leosac::Auth
@brief Holds classes relevant to the Authentication and Authorization subsystem.

This namespace defines interface and concrete implementation of some class
useful in the authentication and authorization system. Authentication module
will likely make direct use of this part of Leosac.
@see @ref auth_main


@namespace Leosac::Module::Auth
@brief Authentication backend modules live here.
