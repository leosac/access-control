Leosac's Modules {#modules}
===========================

Modules are what provides features to Leosac.
Leosac's core load a bunch of modules (as defined in the configuration file) and
everything talks to each other.

See the [Leosac::Module](@ref Leosac::Module) namespace for a reference of all modules.

When configuring a module, the name specified in the configuration file must match the name
provided by the module through its `get_module_name()` function.

Writing a new module {#modules_write}
=====================================

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

It is recommended to derive from [BaseModule](@ref Leosac::Module::BaseModule) to implements your own module.
This base class make things easier. However, it doesn't take care of everything.

There are hard requirements what symbols your shared library (aka modules) must export:
1. A function named `start_module` with the following prototype: `bool start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg, zmqpp::context &zmq_ctx)`
2. A function named `get_module_name` that takes no parameters and return a `const char *` that points to the name of the module. This string must be NULL terminated.
   Note that the returned module name shall be composed of capital letter, number and underscore only.


The `start_module` function can be a one liner if you use the the `BaseModule` class
and the helper [start_module_helper](@ref Leosac::Module::start_module_helper).

@namespace Leosac::Module
@brief All modules that provides features to Leosac shall be in this namespace.
