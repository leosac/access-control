#pragma once

#include <string>
#include <chrono>
#include <zmqpp/socket.hpp>

/**
* A Facade to a LED object.
*
* This object require a valid GPIO object to work. It abstract a LED, and add a simple command on top of GPIO.
* It requires a module that implements the behavior to work properly. However, unlike GPIO modules, we are unlikely
* to have multiple implementation of a LED module.
*
* ### Configuration Requirement
* When defining a LED device in a configuration file, 2 optional configuration parameters shall be available.
*     1. `default_blink_duration` which defines how long a LED will blink if no blink duration is specified.
*     2. `default_blink_speed` which defines how fast a led will blink.
*
* Both theses options are expressed in milliseconds.
*
*
* ### Find below the LED object specifications:
*
* Note that GPIO commands are also valid LED command.
*
* Commands are sent (and response received) using REQ/REP socket. Here is a message specs:
*    1. Frame 1: `COMMAND_NAME`
*    2. Frame 2: `PARAMETER_1`
*    3. Frame 3: `PARAMETER_2`
*
* See command description for more info about parameter.
* - - - - -
*
* We define 5 commands that can be send to a LED device:
*    + `STATE` to query the current state of the device.
*    + `ON` to turn the LED on.
*    + `TOGGLE` the LED: if on if goes off; if off it goes on.
*    + `OFF` to turn the LED off.
*    + `BLINK` so that the LED will blink.
*
* #### `STATE`
* Shall return the state of the LED device. It is a bit more verbose
* than the `STATE` command directed to GPIO device.
* It can return "ON" or "OFF" (in a single frame) if the device is
* simply on or off.
*
* However, if the device is blinking this shall returns a multi
* frames message:
*
*    1. "BLINKING"
*    2. BLINK_DURATION (string)
*    3. BLINK_SPEED (string)
*    4. "ON" | "OFF" (the real current state of the underlying gpio)
*
*
* #### `ON`
* This turns the LED on. It accepts an optional `duration` parameter.
* If set, this parameter express the `duration` for which the LED shall stay on.
* This `duration` is expressed in milliseconds.
*
* The implementation shall turn the LED off after this `duration` has expired.
*
* #### `OFF`
* This turns the LED low. There is no parameter.
*
* #### `TOGGLE`
* Toggle the LED, setting it to low it was set to high, and vice versa. This command
* doesn't expect any parameter either.
*
* #### `BLINK`
* This makes the LED blink, useful for controlling your christmas tree.
* The `BLINK` command accepts 2 optionals parameter: a `duration` and a `speed`. Both are expressed in milliseconds.
*
* The second frame shall contain the duration (use -1 for infinite blink) and the third frame the speed.
*
* @note This may sound stupid, but all value (even numeric one) shall be send as string. This will likely change later.
*/
class FLED
{
public:

    struct State
    {
        State() :
                st(UNKNOWN),
                duration(0),
                speed(0),
                value(false)
        {
        }

        /**
        * Internal state of the LED.
        */
        enum {
            ON,
            OFF,
            BLINKING,
            UNKNOWN,
        } st;

        /**
        * Set only if `st` is `BLINKING`, it represents the total duration of blinking.
        */
        int duration;

        /**
        * Set only if `st` is `BLINKING`, it represents the speed of blinking.
        */
        int speed;

        /**
        * Set only if `st` is `BLINKING` : value of the LED (true if ON, false otherwise).
        */
        bool value;
    };

    FLED(zmqpp::context &ctx, const std::string &led_name);

    /**
    * Disabled copy-constructor.
    * Manually create a new facade using the LED's name instead.
    */
    FLED(const FLED &) = delete;

    /**
    * Default destructor
    */
    ~FLED() = default;

    /**
    * Turn the LED ON and turn it OFF duration milliseconds later.
    */
    bool turnOn(std::chrono::milliseconds duration);

    /**
    * Turn the LED ON by sending a message to the backend LED impl.
    */
    bool turnOn();

    /**
    * Turn the LED OFF by sending a message to the backend LED impl.
    */
    bool turnOff();

    /**
    * Toggle the LED value by sending a message to the backend LED impl.
    */
    bool toggle();

    /**
    * Make the LED blink. No optional parameter so the module shall use the default for the device.
    */
    bool blink();

    /**
    * Blink with a duration and a speed.
    */
    bool blink(std::chrono::milliseconds duration, std::chrono::milliseconds speed);

    bool blink(int duration, int speed);

    /**
    * Query the value of the GPIO and returns true if the LED is ON.
    * It returns false otherwise.
    *
    * If the GPIO is blinking, but currently ON, this returns true.
    */
    bool isOn();

    /**
    * Similar to `isOn()`.
    *
    * If the GPIO is blinking, but currently OFF, this returns true.
    */
    bool isOff();

    /**
    * Returns true is the LED is currently blinking.
    */
    bool isBlinking();

    /**
    * Return the state of the device.
    * See FLED::State for more infos.
    */
    State state();

    /**
    * Access the backend socket (which is connect to the LED device) to send command directly.
    * Use carefully !
    */
    zmqpp::socket &backend();

private:
    /**
    * A socket to talk to the backend LED.
    */
    zmqpp::socket backend_;
};
