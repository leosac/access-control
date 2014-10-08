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

    PFDigitalPin(zmqpp::context &ctx, const std::string &name, int gpio_no);
    ~PFDigitalPin();

    PFDigitalPin(const PFDigitalPin &) = delete;
    PFDigitalPin &operator=(const PFDigitalPin &) = delete;

    PFDigitalPin(PFDigitalPin &&o);
    PFDigitalPin &operator=(PFDigitalPin &&o);

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
    * Set the direction of the GPIO pin. This doesn't really affect the GPIO direction
    * as this is impossible with the Piface.
    * This is here because Piface use 0-7 for INPUT and 0-7 for OUTPUT too. Direction allows us to figure out
    * if this is an output or input pin.
    */
    void set_direction(Direction d);

    Direction direction_;

};
