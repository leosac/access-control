#include "PFDigitalPin.hpp"
#include "mcp23s17.h"
#include "pifacedigital.h"
#include "tools/log.hpp"

PFDigitalPin::PFDigitalPin(zmqpp::context &ctx,
        const std::string &name,
        int gpio_no,
        Direction direction,
        bool value) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(new zmqpp::socket(ctx, zmqpp::socket_type::push)),
        name_(name),
        direction_(direction),
        default_value_(value),
        want_update_(false)
{
    DEBUG("trying to bind to " << ("inproc://" + name));
    sock_.bind("inproc://" + name);
    bus_push_->connect("inproc://zmq-bus-pull");

    if (direction == Direction::Out)
        value ? turn_on() : turn_off();
}

PFDigitalPin::~PFDigitalPin()
{
    if (direction_ == Direction::Out)
        default_value_ ? turn_on() : turn_off();
    delete bus_push_;
}

PFDigitalPin::PFDigitalPin(PFDigitalPin &&o) :
        sock_(std::move(o.sock_)),
        direction_(o.direction_),
        default_value_(o.default_value_)
{
    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    this->bus_push_ = o.bus_push_;
    this->want_update_ = o.want_update_;

    o.bus_push_ = nullptr;
}

void PFDigitalPin::handle_message()
{
    zmqpp::message_t msg;
    std::string frame1;
    sock_.receive(msg);

    msg >> frame1;
    bool ok = false;
    if (frame1 == "ON")
        ok = turn_on(&msg);
    else if (frame1 == "OFF")
        ok = turn_off();
    else if (frame1 == "TOGGLE")
        ok = toggle();
    else if (frame1 == "STATE")
        return send_state();
    else // invalid cmd
        ERROR("Invalid command received (" << frame1 << "). Potential missconfiguration !");
    sock_.send(ok ? "OK" : "KO");
}

bool PFDigitalPin::turn_on(zmqpp::message *msg /* = nullptr */)
{
    if (direction_ != Direction::Out)
        return false;

    if (msg && msg->parts() > 1)
    {
        // optional parameter is present
        int64_t duration;
        *msg >> duration;
        next_update_time_ = std::chrono::system_clock::now() + std::chrono::milliseconds(duration);
        want_update_ = true;
    }
    pifacedigital_digital_write(gpio_no_, 1);

    publish_state();
    return true;
}

bool PFDigitalPin::turn_off()
{
    if (direction_ != Direction::Out)
        return false;
    pifacedigital_digital_write(gpio_no_, 0);

    publish_state();
    return true;
}

bool PFDigitalPin::toggle()
{
    if (direction_ != Direction::Out)
        return false;

    uint8_t v = pifacedigital_read_bit(gpio_no_, OUTPUT, 0);

    if (v)
        pifacedigital_digital_write(gpio_no_, 0);
    else
        pifacedigital_digital_write(gpio_no_, 1);

    publish_state();
    return true;
}

bool PFDigitalPin::read_value()
{
    // pin's direction matter here (not read from same register).
    return pifacedigital_read_bit(gpio_no_, direction_ == Direction::Out ? OUTPUT : INPUT, 0);
}


void PFDigitalPin::update()
{
    DEBUG("UPDATING");
    turn_off();
    want_update_ = false;
}

std::chrono::system_clock::time_point PFDigitalPin::next_update()
{
    if (want_update_)
        return next_update_time_;
    return std::chrono::system_clock::time_point::max();
}

void PFDigitalPin::publish_state()
{
    if (bus_push_)
        bus_push_->send(zmqpp::message() << ("S_" + name_) << (read_value() ? "ON" : "OFF"));
}

void PFDigitalPin::send_state()
{
    sock_.send((read_value() ? "ON" : "OFF"));
}
