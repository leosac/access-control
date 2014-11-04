#pragma once

#include <string>
#include <chrono>
#include <zmqpp/socket.hpp>

/**
* A Facade to a GPIO object.
* This facade does not care about the GPIO implementation. It uses message passing to interact
* with the application "gpio controller".
*
* The backend GPIO object (implemented by the gpio module (either sysfs or piface)) MUST exist.
* All you need is the GPIO name defined in the configuration file to create a facade.
*
* Since Leosac works through message passing, we have to define messages specifications.
*
* ### Find below the GPIO related specs:
*
* Commands are sent (and response received) using REQ/REP socket. Here is a message specs:
*    1. Frame 1: `COMMAND_NAME`
*    2. Frame 2: `PARAMETER_1`
*    3. Frame 3: `PARAMETER_2`
*
* See command description for more info about parameter.
* - - - - -
*
* We define 4 commands that can be send to a GPIO device:
*    + `STATE` to query the state (high / low)
*    + The `ON` command.
*    + `TOGGLE` to inverse the value of a pin
*    + `OFF` to turn the GPIO to low.
*
* #### `STATE`
* This asks for the state of the GPIO pin. It sends its value back, in textual format.
* Therefore it shall always send either "ON" or "OFF".
*
* #### `ON`
* This turns the pin high. It accepts an optional `duration` parameter.
* If set, this parameter express the `duration` for which the GPIO shall stay high.
* This `duration` is expressed in milliseconds.
*
* The implementation shall turn the GPIO off after this `duration` has expired.
*
* #### `OFF`
* This turns the GPIO low. There is no parameter.
*
* #### `TOGGLE`
* Toggle the GPIO, setting it to low it was set to high, and vice versa. This command
* doesn't expect any parameter either.
*
*/
class FGPIO
{
public:
    FGPIO(zmqpp::context &ctx, const std::string &gpio_name);

    /**
    * Disabled copy-constructor.
    * Manually create a new facade using the GPIO's name instead.
    */
    FGPIO(const FGPIO &) = delete;

    /**
    * Default destructor, RAII does the job.
    */
    ~FGPIO() = default;

    /**
    * Turn the GPIO ON and turn it OFF duration milliseconds later.
    */
    bool turnOn(std::chrono::milliseconds duration);

    /**
    * Turn the GPIO ON by sending a message to the backend GPIO impl.
    */
    bool turnOn();

    /**
    * Turn the GPIO OFF by sending a message to the backend GPIO impl.
    */
    bool turnOff();

    /**
    * Toggle the GPIO value by sending a message to the backend GPIO impl.
    */
    bool toggle();

    /**
    * Query the value of the GPIO and returns true if the LED is ON.
    * It returns false otherwise.
    */
    bool isOn();

    /**
    * Similar to `isOn()`.
    */
    bool isOff();

private:
    /**
    * A socket to talk to the backend GPIO.
    */
    zmqpp::socket backend_;
};
