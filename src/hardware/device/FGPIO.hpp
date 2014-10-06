#pragma once

#include <string>
#include <zmqpp/socket.hpp>

/**
* A Facade to a GPIO object.
* This facade does not care about the GPIO implementation. It uses message passing to interract
* with the application "gpio controller".
*
* The backend GPIO object (implemented by the gpio module (either sysfs or piface)) MUST exist.
* All you need is the GPIO name defined in the configuration file to create a facade.
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
    * Turn the GPIO ON by sending a message to the backend GPIO impl.
    */
    bool turnOn();

    /**
    * Turn the GPIO ON by sending a message to the backend GPIO impl.
    */
    bool turnOff();

    /**
    * Turn the GPIO ON by sending a message to the backend GPIO impl.
    */
    bool toggle();

private:
    /**
    * A socket to talk to the backend GPIO.
    */
    zmqpp::socket backend_;
};