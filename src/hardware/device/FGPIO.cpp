#include "FGPIO.hpp"

FGPIO::FGPIO(zmqpp::context &ctx, const std::string &gpio_name) :
backend_(ctx, zmqpp::socket_type::req)
    {
    backend_.bind("inproc://" + gpio_name);
    }

bool FGPIO::turnOn()
    {
    std::string rep;

    backend_.send("ON");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
    }

bool FGPIO::turnOff()
    {
    return false;
    }

bool FGPIO::toggle()
    {
    return false;
    }
