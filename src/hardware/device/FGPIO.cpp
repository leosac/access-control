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
    std::string rep;

    backend_.send("OFF");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
    return false;
}

bool FGPIO::toggle()
{
    std::string rep;

    backend_.send("TOGGLE");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}
