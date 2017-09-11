@page page_module_intro An introduction to modules

Leosac's Modules {#modules}
===========================

[TOC]

@brief An introduction to modules.

This pages aims to be an introduction to modules in Leosac, both for the non-technical end-user
and for developers. The developers part assumes the end-user was read and understood.

A brief introduction {#modules_brief_intro}
===========================================

Modules are what provides features to Leosac: devices supports, authentication backend,
monitoring, all are implemented through modules.
Modules are cool because they allows to build the configuration you want by enabling
only the feature you will use. It also makes it easy to expand (or reimplement)
the existing functionally.

<HR>

As an end-user, what should I know? {#modules_enduser}
============================================================

You should know (or be able to find out) what modules you need for your use case.
You then need to be able to properly configure them.

Using and configuring modules {#modules_enduser_use}
-------------------------------------------------------------

Where to find modules, which modules to load, and their basic configuration take
place in the the `core configuration file`. You can read-up a per-module documentation
that will detail and explain how to configure the module.

Some module may have additional configuration files in order to not bloat the main
config file.

What modules do I need? {#modules_enduser_what}
-----------------------------------------------

It's not really possible to answer this, because it depends heavily and how you plan
to use Leosac. However, if you're doing access-control you will generally need:
   1. A module to provide support for GPIO pins.
   2. The module that implements support for Wiegand device.
   3. An authentication backend module that will determine whether an user shall be granted access or not.
   4. A module to take action when an authentication attempt happen. This is the [doorman](@ref mod_doorman_main) module.

Overview of existing modules {#module_enduser_overview}
-------------------------------------------------------

Name                 | Description                                                   | More info
---------------------|---------------------------------------------------------------|--------------
SysFS GPIO           | Provide support for GPIO through SysFS                        | Hardware module
PifaceDigital        | Provide support for GPIO through the Piface digital device    | Hardware module
Led-Buzzer           | Support Led and buzzer devices.                               | Hardware module, requires a GPIO module
Wiegand              | Support wiegand device.                                       | Hardware module, Authentication Source module. Requires GPIO module.
Auth-File            | Implementation permissions checking / auth validation         | Authentication backend module.
Doorman              | Take actions when an authentication attempt succeed or fails  | Policy implementation module (takes actions on event)
Replication          | Allows Leosac to run in a master/slave configuration          | -
Monitor              | Additional logs. Provide activity feedback (is network down?) | -

<HR>

Developers Informations {#modules_dev}
======================================

The following section is aimed toward developers.

Leosac's core loads a bunch of modules (as defined in the configuration file) and
everything talks to each other.

See the [Leosac::Module](@ref Leosac::Module) namespace for a reference of all modules.

When configuring a module, the name specified in the configuration file must match the name
provided by the module through its `get_module_name()` function.

How modules communicate ? {#modules_communication}
--------------------------------------------------

Modules to modules communication and core to modules communication does **NOT** happen
by invoking methods on each other. Instead, we have a higher level of abstraction in place:
we use message passing. Each module has a mailbox and can receive (and send)
messages to anyone.

For this to work, proper specifications must be in place, so that we can swap an
implementation with an other without changing anything else in dependants modules. For example,
we can use either the Piface device (and the Piface module) to implements GPIO, but we could
also use the SysfsGPIO module. Other modules that needs to use GPIO do not care which GPIO module
is loaded and provide the support for the GPIOs.

Leosac's core also set up a message-bus where everyone can write and read from everyone else.
This is useful to broadcast information and is used a lot.

![Modules communications](../resources/modules_communication.png)

Writing a new module {#modules_write}
-------------------------------------

To write a new module you need to code either in C++ or eventually in C.
There are a few guidelines (some are even requirements) that you have to follow in order
to write a module compliant with the Leosac system.

If you're going to write a module to handle hardware devices (leds, gpios, etc) take a look at
the current implementation and to the specifications of said device:
+ [GPIO](@ref Leosac::Hardware::FGPIO)
+ [LED](@ref Leosac::Hardware::FLED)
+ [Wiegand Reader](@ref Leosac::Hardware::FWiegandReader)

Tips {#modules_write_tips}
--------------------------

It is recommended to derive from [BaseModule](@ref Leosac::Module::BaseModule) to implement your own module.
This base class make things easier. However, it doesn't take care of everything.

There are hard requirements regarding which symbols your shared library (aka modules) must export:
1. A function named `start_module()` with the following prototype:
   `bool start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg, zmqpp::context &zmq_ctx, CoreUtilsPtr utils)`
2. A function named `get_module_name()` that takes no parameters and return a `const char *` that points to the name of the module. This string must be `NULL` terminated.
   Note that the returned module name shall be composed of capital letter, number and underscore only.


The `start_module()` function can be a one liner if you use the the `BaseModule` class
and the helper [start_module_helper](@ref Leosac::Module::start_module_helper).

@namespace Leosac::Module
@brief All modules that provides features to Leosac shall be in this namespace.
