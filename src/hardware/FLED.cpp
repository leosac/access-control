#include "FLED.hpp"
#include <zmqpp/message.hpp>

using namespace Leosac::Hardware;

FLED::FLED(zmqpp::context &ctx, const std::string &led_name) :
        backend_(ctx, zmqpp::socket_type::req)
{
    backend_.connect("inproc://" + led_name);
}

bool FLED::turnOn(std::chrono::milliseconds duration)
{
    std::string rep;
    zmqpp::message msg;

    msg << "ON" << std::to_string(duration.count());

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
    State st = state();

    if (st.st == State::BLINKING)
        return st.value;
    if (st.st == State::OFF)
        return false;
    if (st.st == State::ON)
        return true;
    assert(0);
}

bool FLED::isOff()
{
    return !isOn();
}

bool FLED::isBlinking()
{
    return state().st == State::BLINKING;
}

FLED::State FLED::state()
{
    FLED::State led_state;

    zmqpp::message rep;
    std::string status_str;

    backend_.send("STATE");
    backend_.receive(rep);

    rep >> status_str;
    if (status_str == "BLINKING")
    {
        assert(rep.parts() == 4);
        led_state.st = State::BLINKING;

        // duration
        rep >> status_str;
        led_state.duration = std::stoi(status_str);

        //speed
        rep >> status_str;
        led_state.speed = std::stoi(status_str);

        // value
        rep >> status_str;
        assert(status_str == "ON" || status_str == "OFF");
        led_state.value = status_str == "ON";
    }
    else
    {
        assert(rep.parts() == 1);
        if (status_str == "ON")
            led_state.st = State::ON;
        else if (status_str == "OFF")
            led_state.st = State::OFF;
        else
            assert (0);
    }
    return led_state;
}

zmqpp::socket &FLED::backend()
{
    return backend_;
}
