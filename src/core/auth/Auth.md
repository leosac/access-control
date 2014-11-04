Authentication Documentation {#auth_main}
============================================

[TOC]

[AccessProfile]: @ref Leosac::Auth::IAccessProfile "Access Profile"
[Cards]: @ref Leosac::Auth::ICard "Card"
[Users]: @ref Leosac::Auth::IUser "User"

Introduction {#auth_intro}
=============================

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
    

@namespace Leosac::Auth
@brief Holds classes relevant to the Authentication and Authorization subsystem.

This namespace defines interface and concrete implementation of some class
useful in the authentication and authorization system. Authentication module
will likely make direct use of this part of Leosac.
@see @ref auth_main
