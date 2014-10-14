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

    msg << "BLINK" << std::to_string(duration.count()) << std::to_string(speed.count());

    backend_.send(msg);
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink(int duration, int speed)
{
    return blink(std::chrono::milliseconds(duration), std::chrono::milliseconds(speed));
}

bool FLED::isOn()
{
    zmqpp::message rep;
    std::string st;

    backend_.send("STATE");
    backend_.receive(rep);

    rep >> st;
    if (st == "BLINKING")
    {
        assert(rep.parts() == 4);
        // duration / speed / "real state"
        rep >> st >> st >> st;
    }
    else
        assert(rep.parts() == 1);
    if (st == "ON")
        return true;
    if (st == "OFF")
        return false;
    assert(0);
}

bool FLED::isOff()
{
    return !isOn();
}
