#pragma once

#include <zmqpp/zmqpp.hpp>
#include <string>

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
    * Write to PFDigital to turn the gpio on.
    */
    bool turn_on();

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

};
