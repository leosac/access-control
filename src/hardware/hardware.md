Hardware Management {#hardware_management}
==========================================

[TOC]

Introduction {#hardware_mangement_intro}
========================================

Since the main goal of Leosac is to handle access control of physical object (mainly doors), and
because it reads data from specific hardware (various wiegand reader) we can say that the hardware
plays an important role in the project.

We must be able to interface with various devices, in various setup. A strong abstraction of the 
hardware is in place. This allows easy and flexible configuration for the end-user, and help with
maintaining and expanding the code.

A Module based architecture
===========================

As you probably already know, Leosac is built around modules (aka shared library). 
Leosac module are more than just plugins. They do not extend core functionalities, they **provide** them.
If you do not start any module, Leosac will be useless and will literally sleep forever.

The core infrastructure is designed to support modules and provide facilities for them to communicate
with each other. Modules communicate using message-passing: [libzmq](http://zeromq.org) (and the [C++ binding: zmqpp](https://github.com/zeromq/zmqpp))
are used to implement this message passing. Various specifications about how messages are supposed to be formatted and
the expected interaction between components are defined through the project.


An example of hardware management
---------------------------------

So, let's see how this module based architecture impact hardware management. Let's suppose we want to
control a Wiegand card reader: This means that we must be able to read a card number from it, and that
must be able to control the device's led and buzzer.

We can see our wiegand reader as multiple sub-components:
   + A LED.
   + A buzzer.
   + A `data0` input PIN.
   + A `data1` input PIN.

We can go a bit lower and identify other sub-components:
   + A LED...
      1. ... that relies on a GPIO output PIN.
   + A buzzer...
      1. ... that relies on a GPIO output PIN too.
   + A `data0` input PIN.
   + A `data1` input PIN.

`data0` and `data1` are GPIO input PIN already.


@namespace Leosac::Hardware
@brief Provides facade classes to hardware device implementation.

Classes in this namespace are classes that hide message-passing details
to their caller.

Since devices support is implemented through module, we need to send message
to communicate with the device. Those classes provide a nice abstraction
to do so with traditional method call instead of manually sending message.

@note It is recommended to use theses classes to interact with a device.
