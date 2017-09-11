@page page_hardware Hardware Management

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

This is a graphic overview of hardware modules. The GPIO module can be changed without impacting the other
modules on top if it, provided that it properly implements the specifications.

![Modules Images](../resources/hardware.png)
@image latex "../resources/hardware.png"


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

<HR>


Hardware modules specifications {#hardware_mod_spec}
====================================================

You can find below the specifications that must be implemented when writing a module that provide
support for some kind of hardware. 

Specification for GPIOs modules {#hardware_spec_gpio}
-----------------------------------------------------

A module that implements GPIOs support must:
    1. Allow user to configure `input` / `output` pin and let them associate a name per GPIO.
    2. Allow other modules to talk to each GPIO individually, by name.
    
A facade object, [FGPIO](@ref Leosac::Hardware::FGPIO) implements client-code (requests) of those specs, and
the module must implement the server-code (responses).

We define 4 commands that can be send to a GPIO device:
    + [STATE](@ref hardware_spec_gpio_state).
    + [ON](@ref hardware_spec_gpio_on).
    + [OFF](@ref hardware_spec_gpio_off).
    + [TOGGLE](@ref hardware_spec_gpio_toggle).

### STATE {#hardware_spec_gpio_state}
 
This is used to query the state of the GPIO pin.

_Request_:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "STATE"                                       | `string`

_Response_:
 
Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "ON" or "OFF"                                 | `string`


### ON {#hardware_spec_gpio_on}

This turns the pin high. It accepts an optional `duration` parameter.
If set, this parameter express the `duration` for which the GPIO shall stay high.
This `duration` is expressed in milliseconds.
The module shall turn the GPIO off after this `duration` has expired.

_Request_:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "ON"                                          | `string`
2        | 3500                                          | `int64_t`

_Response_:
 
Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "OK" or "KO"                                  | `string`

@note While it would return "KO" on error, it shouldn't fail, ever.


### OFF {#hardware_spec_gpio_off}

This turns the GPIO low. There is no parameter.

_Request_:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "OFF"                                         | `string`

_Response_:
 
Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "OK" or "KO"                                  | `string`

@note While it would return "KO" on error, it shouldn't fail, ever.


### TOGGLE {#hardware_spec_gpio_toggle}

Toggle the GPIO, setting it to low it was set to high, and vice versa. This command takes no parameter.

_Request_:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "TOGGLE"                                      | `string`

_Response_:
 
Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "OK" or "KO"                                  | `string`

@note While it would return "KO" on error, it shouldn't fail, ever.

<HR>

Specification for LEDs modules {#hardware_spec_led}
----------------------------------------------------

A module that implements LED devices support must:
    1. Allow user to identified LED by name.
    2. Allows module to communicate with configured LED device, by name.
    3. Let the user chose a default

Note that some GPIO commands are also valid LED command.

We define 5 commands that can be send to a LED device:
   + [STATE](@ref hardware_spec_led_state).
   + [ON](@ref hardware_spec_gpio_on). This works exactly like the `ON` command for GPIO.
   + [OFF](@ref hardware_spec_gpio_off). This works exactly like the `ON` command for GPIO.
   + [TOGGLE](@ref hardware_spec_gpio_toggle). This works exactly like the `TOGGLE` command for GPIO.
   + [BLINK](@ref hardware_spec_led_blink).

### STATE {#hardware_spec_led_state}

Shall return the state of the LED device. 

_Request_:

Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------
1        | "STATE"                               | `string`

_Response_:
 
Frame    | Content                               | Type        | Comment
---------|---------------------------------------|-------------|------------------------------
1        | "ON" or "OFF" or "BLINKING"           | `string`    | -
2        | BLINK_DURATION                        | `int64_t`   | Only if frame 1 is "BLINKING"
3        | BLINK_SPEED                           | `int64_t`   | Only if frame 1 is "BLINKING"
4        | "ON" or "OFF"                         | `string`    | Only if frame 1 is "BLINKING" - indicate the current value when the command was received.


### BLINK {#hardware_spec_led_blink}

This makes the LED blink, useful for controlling your christmas tree.
The `BLINK` command accepts 2 optionals parameter: a `duration` and a `speed`. Both are expressed in milliseconds.

The second frame shall contain the duration (use -1 for infinite blink) and the third frame the speed.

_Request_:

Frame    | Content                               | Type      | Comment
---------|---------------------------------------|-----------|--------------------------------
1        | "BLINK"                               | `string`  | -
2        | BLINK_DURATION                        | `int64_t` | Optional. In milliseconds.
3        | BLINK_SPEED                           | `int64_t` | Optional. In milliseconds.

_Response_:
 
Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------
1        | "OK" or "KO"                                  | `string`

@note While it would return "KO" on error, it shouldn't fail, ever.

<HR>

Specification for Buzzer modules {#hardware_spec_buzzer}
----------------------------------------------------

Buzzer and LED share the same code. See the [led module specifications](@ref hardware_spec_led).


<HR>

Specification for Wiegand module {#hardware_spec_wiegand}
----------------------------------------------------------

A wiegand reader device shall have 2 inputs GPIO configured and optional greenled and buzzer
devices.

### Command specifications:

A wiegand device shall accept 4 commands:
   + [BEEP](@ref hardware_spec_wiegand_beep).
   + [BEEP_ON](@ref hardware_spec_wiegand_beep_on).
   + [BEEP_OFF](@ref hardware_spec_wiegand_beep_off).
   + [GREEN_LED](@ref hardware_spec_wiegand_greenled).


### BEEP {#hardware_spec_wiegand_beep}

Make the reader beep for a given duration. The duration is expressed in milliseconds.

_Request_:

Frame    | Content                               | Type      | Comment
---------|---------------------------------------|-----------|-------------------------------
1        | "BEEP"                                | `string`  | -
2        | BEEP_DURATION                         | `int64_t` | In milliseconds.

_Response_:
 
Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------
1        | "OK" or "KO"                          | `string`

@note Would return "KO" if the reader had no associated buzzer (or if the underlying GPIO returned KO -- unlikely).


### BEEP_ON {#hardware_spec_wiegand_beep_on}

Turn the buzzer of the reader on. No duration.

_Request_:

Frame    | Content                               | Type      | Comment
---------|---------------------------------------|-----------|-------------------------------
1        | "BEEP_ON"                                | `string`  | -

_Response_:
 
Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------
1        | "OK" or "KO"                          | `string`

@note Would return "KO" if the reader had no associated buzzer (or if the underlying GPIO returned KO -- unlikely).


### BEEP_OFF {#hardware_spec_wiegand_beep_off}

Turn the buzzer of the reader off.

_Request_:

Frame    | Content                               | Type      | Comment
---------|---------------------------------------|-----------|-------------------------------
1        | "BEEP_OFF"                            | `string`  | -

_Response_:
 
Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------
1        | "OK" or "KO"                          | `string`

@note Would return "KO" if the reader had no associated buzzer (or if the underlying GPIO returned KO -- unlikely).


### GREEN_LED {#hardware_spec_wiegand_greenled}

Forward a command to the greenled device of the reader.

_Request_:

Frame    | Content                               | Type      | Comment
---------|---------------------------------------|-----------|-------------------------------
1        | "GREEN_LED"                           | `string`  | -
2        | xxx                                   | `xxx`     | Command for the LED device.
3        | yyy                                   | `yyy`     | Parameter 1 for the command. Can have more, or none.

_Response_:
 
Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------
1        | "OK" or "KO"                          | `string`

@note Would return "KO" if the reader had no associated green led.

<HR>

@namespace Leosac::Hardware
@brief Provides facade classes to hardware device implementation.

Classes in this namespace are classes that hide message-passing details
to their caller.

Since devices support is implemented through module, we need to send message
to communicate with the device. Those classes provide a nice abstraction
to do so with traditional method call instead of manually sending message.

@note It is recommended to use theses classes to interact with a device.
