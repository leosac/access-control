#include "FLED.hpp"
#include <zmqpp/message.hpp>

FLED::FLED(zmqpp::context &ctx, const std::string &led_name) :
        backend_(ctx, zmqpp::socket_type::req)
{
    backend_.connect("inproc://" + led_name);
}

bool FLED::turnOn(std::chrono::milliseconds duration)
{
    std::string rep;
    zmqpp::message msg;

    msg << "ON" << duration.count();

    backend_.send(msg);
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::turnOn()
{
    std::string rep;

    backend_.send("ON");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::turnOff()
{
    std::string rep;

    backend_.send("OFF");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::toggle()
{
    std::string rep;

    backend_.send("TOGGLE");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink()
{
    std::string rep;

    backend_.send("BLINK");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink(std::chrono::milliseconds duration, std::chrono::milliseconds speed)
{
    std::string rep;
    zmqpp::message msg;

    msg << "BLINK" << duration.count() << speed.count();

    backend_.send(msg);
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}
