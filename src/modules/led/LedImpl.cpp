#include "LedImpl.hpp"
#include "tools/log.hpp"

LedImpl::LedImpl(zmqpp::context &ctx,
        std::string const &led_name,
        std::string const &gpio_name,
        int blink_duration,
        int blink_speed) :
ctx_(ctx),
frontend_(ctx, zmqpp::socket_type::rep),
backend_(ctx, zmqpp::socket_type::req),
default_blink_duration_(blink_duration),
default_blink_speed_(blink_speed),
want_update_(false),
gpio_(ctx, gpio_name)
{
    frontend_.bind("inproc://" + led_name);
    backend_.connect("inproc://" + gpio_name);
}

zmqpp::socket &LedImpl::frontend()
{
    return frontend_;
}

void LedImpl::handle_message()
{
    zmqpp::message_t msg;
    zmqpp::message_t rep;
    std::string frame1;

    frontend_.receive(msg);
    msg >> frame1;
    bool ok = false;
    if (frame1 == "ON" || frame1 == "OFF" || frame1 == "TOGGLE")
    {
        // simply forward message to GPIO
        rep = send_to_backend(msg);
        frontend_.send(rep);
        return;
    }
    else if (frame1 == "BLINK")
        ok = start_blink(&msg);
    else // invalid cmd
        ok = false;
    frontend_.send(ok ? "OK" : "KO");
}

void LedImpl::update()
{
    LOG() << "UPDATING LED";

    gpio_.toggle();
    next_update_time_ = std::chrono::system_clock::now() + std::chrono::milliseconds(blink_speed_);
    if (std::chrono::system_clock::now() > blink_end_)
    {
        want_update_ = false;
        gpio_.turnOff();
    }
}

std::chrono::system_clock::time_point LedImpl::next_update()
{
    if (want_update_)
        return next_update_time_;
    return std::chrono::system_clock::time_point::max();
}

zmqpp::message LedImpl::send_to_backend(zmqpp::message &msg)
{
    zmqpp::message rep;
    backend_.send(msg);

    backend_.receive(rep);
    return rep;
}

bool LedImpl::start_blink(zmqpp::message *msg)
{
    assert(msg->parts() > 2);
    if (msg->parts() > 2)
    {
        std::string duration;
        std::string speed;

        *msg >> duration;
        *msg >> speed;
        blink_speed_  = std::stoi(speed);

        assert(speed < duration);

        blink_end_ = std::chrono::system_clock::now() + std::chrono::milliseconds(std::stoi(duration));
        next_update_time_ = std::chrono::system_clock::now() + std::chrono::milliseconds(blink_speed_);
        want_update_ = true;
        //send_to_backend(zmqpp::message() << "TOGGLE");
        gpio_.toggle();
        return true;
    }
    return false;
}
