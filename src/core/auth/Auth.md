Authentication Documentation {#auth_main}
============================================

[TOC]

[AccessProfile]: @ref Leosac::Auth::IAccessProfile "Access Profile"
[Cards]: @ref Leosac::Auth::ICard "Card"
[Users]: @ref Leosac::Auth::IUser "User"

Introduction {#auth_intro}
=============================

rewrite this.

In an access control system, the authentication subsystem is complex.
This page aims to explain how it works in Leosac.

This is a job shared by authentication module (their job is to extract data, and
create decent mapping between [AccessProfile], [Users] and [Cards]. Leosac core, through \n
the implementation of those class provide facility to authorize (or deny) the user. \n
We can somehow say that Authentication Module will **authenticate** while the core
classes will **authorize**


The [AccessProfile] interface define a base class for implementing access control rules.

It is the responsibility of various authentication modules to build a coherent
chains of [Cards], [Users], and [AccessProfile].

Once this "building" is done, the [AccessProfile] class shall have all it needs
to perform the access check.


Workflow:
    1. Send raw data (module wiegand)
    2. Build [Cards] from input data (auth module code).
    3. Maps cards to [Users]. (auth module).
    3.1 If card is temporary, map to access profile.
    4. Map [Users] to [AccessProfile]. (auth module)
    

Two kinds of modules
====================

We break down the authentication & authorization code in multiple parts.
There are two kinds of modules:
    1. Authentication Source module (eg [Wiegand](@ref @mod_wiegand_main))
    2. Authentication Backend module (eg [Files](@ref @mod_authfile_main))

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

TODO:
( Who checks the profiles?? TBD)

In order for this to work properly, we need a decent message passing specifications.

Authentication data type {#auth_data_type}
------------------------------------------

Data types information helps authentication backend understand message from authentication sources.
It is an enumeration value, see [AuthSourceType](@ref Leosac::Auth::SourceType).

Currently we are lucky, we only define one.

1. **SIMPLE_WIEGAND** : This data type requires 2 frames:
  1. A hexadecimal *string* that contain the card id that was read, for example: "40:a0:83:80"
  1. An *integer* that holds the number of bits in the card id. 
  

Message Passing Specifications {#auth_specc}
============================================

**This is not implemented yet**.

Understand the [application message bus](@ref MessageBus) first.

First, a few assumptions. Authentication data from auth source may vary widely.
Consider a simple pin code versus a wiegand card id. The wiegand card must holds 2 data: the value, and the
number of meaningful bits.
A wiegand reader can also handle PIN code + card id, augmenting complexity of the auth data.

Auth backend must be able to distinguish between those to handles them properly.
 

Workflow for an **authentication source** module:
 1. Having enough meaningful data that represents an authentication source (card id, pin code, fingerprint, ...)
    the module shall publish the message bus the let other know.
 2. The message shall be multiparts.
    1. Its first frame must be the auth source's name prefixed by "S_".
    2. The second frame shall be the type of data. See [this](@ref auth_data_type).    
    3. Optional (but likely presents) frames that contains different data based on the [auth data type](@ref auth_data_type)


As an example, consider a message from the [wiegand module](@ref mod_wiegand_main), named `MY_WIEGAND_1`:
 1. "S_MY_WIEGAND_1"
 2. `Leosac::Auth::SourceType::SIMPLE_WIEGAND`
 3. "12:af:cd:21"
 4. `26`
 
 Frames 1 and 3 are string. Frame 2 is an enum: [AuthSourceType](@ref Leosac::Auth::SourceType). Frame 4 is integer.


Workflow for an **authentication backend** module:
 1. Receive a message from an **auth source**.
 2. Process the message, and decide whether access shall be granted or not.
 3. Send a multiparts response message.
   1. The first frame MUST be "S_{AUTH_INSTANCE_NAME}".
   2. The second frame shall be an [AccessStatus](@ref Leosac::Auth::AccessStatus).

Example:
 1. "S_MY_AUTH_SOURCE_1"
 2. `Leosac::Auth::SourceType::Granted`
 

@namespace Leosac::Auth
@brief Holds classes relevant to the Authentication and Authorization subsystem.

This namespace defines interface and concrete implementation of some class
useful in the authentication and authorization system. Authentication module
will likely make direct use of this part of Leosac.
@see @ref auth_main
