Hardware Management {#hardware_management}
==========================================

[TOC]

@brief An explanation of how hardware modules work together.

Introduction {#hardware_management_intro}
========================================

Since the main goal of Leosac is to handle access control of physical object (mainly doors), and
because it reads data from specific hardware (various wiegand reader) we can say that the hardware
plays an important role in the project.

We must be able to interface with various devices, in various setup. A strong abstraction of the 
hardware is in place. This allows easy and flexible configuration for the end-user, and help with
maintaining and expanding the code.

A Module based architecture {#hardware_management_mba}
=====================================================

As you probably already know, Leosac is built around modules (aka shared library). 
Leosac module are more than just plugins. They do not extend core functionalities, they **provide** them.
If you do not start any module, Leosac will be useless and will literally sleep forever.

The core infrastructure is designed to support modules and provide facilities for them to communicate
with each other. Modules communicate using message-passing: [libzmq](http://zeromq.org) (and the [C++ binding: zmqpp](https://github.com/zeromq/zmqpp))
are used to implement this message passing. Various specifications about how messages are supposed to be formatted and
the expected interaction between components are defined through the project.


An example of hardware management {#hardware_management_example}
----------------------------------------------------------------

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

There are 4 kind of hardware here: GPIO, Led, Buzzer and Wiegand Reader. Led and Buzzer are implemented as one.
We have (at least) one module per device type. A module for a specific hardware provide support for it.

The LED module requires that a GPIO module exposes the GPIO to the application in order to work. Same goes for the
Wiegand module.
GPIO can be provided by 2 modules: Through the PifaceDigital card, or through sysfs. The other do not care
what module handle the GPIO. As long as one of them do, and respects the specifications.

So, in our example, here is what happens when a card is presented to the reader:
   1. The hardware GPIO will emit some GPIO interrupt.
   2. The GPIO module (doesn't matter which one) will read those interrupt and present them to the application as messages. (remember, we heavily use message passing)
   3. The Wiegand Module will read the GPIO interrupt from the GPIO module (in the same way, independently of the underlying GPIO module) and will build a card number from those.
      This information is then sent to the authentication module.
   4. The authentication module chose whether the access shall be granted or not, and publish this information.
   5. The `doorman` module picks this up, and eventually open a door.

@namespace Leosac::Hardware
@brief Provides facade classes to hardware device implementation.

Classes in this namespace are classes that hide message-passing details
to their caller.

Since devices support is implemented through module, we need to send message
to communicate with the device. Those classes provide a nice abstraction
to do so with traditional method call instead of manually sending message.

@note It is recommended to use theses classes to interact with a device.
