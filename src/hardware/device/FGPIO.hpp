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

    bool turnOn();
    bool turnOff();
    bool toggle();

private:
    /**
    * A socket to talk to the backend GPIO.
    */
    zmqpp::socket backend_;
    };