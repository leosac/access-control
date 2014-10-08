#pragma once

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <chrono>

/**
* This is a implementation class. It's not exposed to the user and is for this
* module internal code only.
*
* It abstract a GPIO pin driven by the PifaceDigital card.
*/
struct PFDigitalPin
{
    enum class Direction {
        In = 0,
        Out
    };

    /**
    * Create a new GPIO pin.
    *
    * @param ctx The ZeroMQ context
    * @param name the name we defined for the PIN
    * @param gpio_no The number of this GPIO
    * @param direction Whether this an input or output pin.
    * @param value the initial value of the pin. This only make sense if the pin is an output pin.
    */
    PFDigitalPin(zmqpp::context &ctx,
            const std::string &name,
            int gpio_no,
            Direction direction,
            bool value);

    ~PFDigitalPin();

    PFDigitalPin(const PFDigitalPin &) = delete;
    PFDigitalPin &operator=(const PFDigitalPin &) = delete;

    PFDigitalPin(PFDigitalPin &&o);
    PFDigitalPin &operator=(PFDigitalPin &&) = delete;

    /**
    * Let the GPIO pin perform internal task.
    * This is called by the PFDigitalModule in the main loop.
    *
    * @note For example we use this to perform "auto turn off" after receiving a "ON" command with a `duration` parameter.
    * @note the previous example is the only use case for now, so we lack code determining what to do on update.
    */
    void update();

    /**
    * This method shall returns the number of the time point at which we want to be updated.
    * If you do not want to be updated, return time_point::max() basically making sure we wont be called.
    */
    std::chrono::system_clock::time_point next_update();

    /**
    * Write to PFDigital to turn the gpio on.
    * @param msg optional pointer to the source message. We can extract optional parameter, if any
    */
    bool turn_on(zmqpp::message *msg = nullptr);

    /**
    * Write to PFDigital turn the gpio off.
    */
    bool turn_off();

    bool toggle();

    /**
    * The PFGpioModule will register this method so its called when a message
    * is ready on the pin socket.
    */
    void handle_message();

    int gpio_no_;

    /**
    * listen to command from other component.
    */
    zmqpp::socket sock_;

    /**
    * PUSH socket to write to the bus.
    */
    zmqpp::socket bus_push_;

    std::string name_;

    /**
    * Ask the PiFace device for this pin's value and return it.
    */
    bool read_value();

    /**
    * This is the direction of the GPIO pin.
    * This cannot be modified once the GPIO has been created.
    */
    const Direction direction_;

    /**
    * The default value of the pin, it is only relevant is the pin is output.
    * We store this to reset the pin its initial state upon unloading the module.
    */
    bool default_value_;

    /**
    * Time point of next wished update.
    */
    std::chrono::system_clock::time_point next_update_time_;

    /**
    * Does this object wants to be `update()`d ?
    */
    bool want_update_;
};
