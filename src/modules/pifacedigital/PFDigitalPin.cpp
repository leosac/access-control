#include "PFDigitalPin.hpp"
#include "mcp23s17.h"
#include "pifacedigital.h"
#include "tools/log.hpp"

PFDigitalPin::PFDigitalPin(zmqpp::context &ctx, const std::string &name, int gpio_no) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(ctx, zmqpp::socket_type::push),
        name_(name)
{
    LOG() << "trying to bind to " << ("inproc://" + name);
    sock_.bind("inproc://" + name);
    bus_push_.connect("inproc://zmq-bus-pull");
}

PFDigitalPin::~PFDigitalPin()
{

}

PFDigitalPin::PFDigitalPin(PFDigitalPin &&o) :
        sock_(std::move(o.sock_)),
        bus_push_(std::move(o.bus_push_))
{
    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    this->direction_ = o.direction_;
}

PFDigitalPin &PFDigitalPin::operator=(PFDigitalPin &&o)
{
    sock_ = std::move(o.sock_);
    bus_push_ = std::move(o.bus_push_);

    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    this->direction_ = o.direction_;

    return *this;
}


void PFDigitalPin::handle_message()
{
    zmqpp::message_t msg;
    std::string frame1;
    sock_.receive(msg);

    msg >> frame1;
    bool ok = false;
    if (frame1 == "ON")
        ok = turn_on();
    else if (frame1 == "OFF")
        ok = turn_off();
    else if (frame1 == "TOGGLE")
        ok = toggle();
    else // invalid cmd
        ok = false;
    sock_.send(ok ? "OK" : "KO");

    // publish new state.
    bus_push_.send(zmqpp::message() << ("S_" + name_) << (read_value() ? "ON" : "OFF"));
}

bool PFDigitalPin::turn_on()
{
    if (direction_ != Direction::Out)
        return false;
    pifacedigital_digital_write(gpio_no_, 1);
    return true;
}

bool PFDigitalPin::turn_off()
{
    if (direction_ != Direction::Out)
        return false;
    pifacedigital_digital_write(gpio_no_, 0);
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
    return true;
}


void PFDigitalPin::set_direction(PFDigitalPin::Direction d)
{
    direction_ = d;
}

bool PFDigitalPin::read_value()
{
    // pin's direction matter here (not read from same register).
    return pifacedigital_read_bit(gpio_no_, direction_ == Direction::Out ? OUTPUT : INPUT, 0);
}

