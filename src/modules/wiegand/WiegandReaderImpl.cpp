#include <iomanip>
#include "WiegandReaderImpl.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Hardware;

WiegandReaderImpl::WiegandReaderImpl(zmqpp::context &ctx,
        const std::string &name,
        const std::string &data_high_pin,
        const std::string &data_low_pin,
        const std::string &green_led_name,
        const std::string &buzzer_name) :
        bus_sub_(ctx, zmqpp::socket_type::sub),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(ctx, zmqpp::socket_type::push),
        counter_(0),
        name_(name),
        green_led_(nullptr),
        buzzer_(nullptr),
        reverse_mode_(false)
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_push_.connect("inproc://zmq-bus-pull");

    sock_.bind("inproc://" + name);

    topic_high_ = "S_INT:" + data_high_pin;
    topic_low_ = "S_INT:" + data_low_pin;

    bus_sub_.subscribe(topic_high_);
    bus_sub_.subscribe(topic_low_);

    std::fill(buffer_.begin(), buffer_.end(), 0);

    if (!green_led_name.empty())
        green_led_ = new FLED(ctx, green_led_name);

    if (!buzzer_name.empty())
        buzzer_ = new FLED(ctx, buzzer_name);
}

WiegandReaderImpl::~WiegandReaderImpl()
{
    delete green_led_;
    delete buzzer_;
}

WiegandReaderImpl::WiegandReaderImpl(WiegandReaderImpl &&o) :
        bus_sub_(std::move(o.bus_sub_)),
        sock_(std::move(o.sock_)),
        bus_push_(std::move(o.bus_push_)),
        name_(std::move(o.name_)),
        reverse_mode_(o.reverse_mode_)
{
    topic_high_ = o.topic_high_;
    topic_low_ = o.topic_low_;

    buffer_ = o.buffer_;
    counter_ = o.counter_;
    green_led_ = o.green_led_;
    buzzer_ = o.buzzer_;
    reverse_mode_ = o.reverse_mode_;

    o.green_led_ = nullptr;
    o.buzzer_ = nullptr;
}

void WiegandReaderImpl::handle_bus_msg()
{
    std::string msg;
    bus_sub_.receive(msg);

    if (counter_ < 128)
    {
        if (msg == topic_high_)
        {
            buffer_[counter_ / 8] |= (1 << (7 - counter_ % 8));
        }
        else if (msg == topic_low_)
        {
            // well, nothing to do here.
        }
        counter_++;
    }
    else
    {
        WARN("Received too many interrupt. Resetting current counter.");
        counter_ = 0;
    }
}

void WiegandReaderImpl::timeout()
{
    if (!counter_)
        return;

    DEBUG("timeout, buffer size = " << counter_);
    std::size_t size = ((counter_ - 1) / 8) + 1;

    std::stringstream card_hex;
    if (reverse_mode_)
    {
        // we need to reverse the order
        for (std::size_t i = 0; i < size; ++i)
        {
            card_hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(buffer_[size - i - 1]);
            if (i + 1 < size)
                card_hex << ":";
        }
    }
    else
    {
        for (std::size_t i = 0; i < size; ++i)
        {
            card_hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(buffer_[i]);
            if (i + 1 < size)
                card_hex << ":";
        }
    }

    zmqpp::message msg;
    msg << ("S_" + name_) << card_hex.str() << counter_;
    bus_push_.send(msg);

    std::fill(buffer_.begin(), buffer_.end(), 0);
    counter_ = 0;
}

void WiegandReaderImpl::handle_request()
{
    zmqpp::message msg;
    std::string str;
    sock_.receive(msg);

    msg >> str;
    assert(str == "GREEN_LED" || str == "BEEP" || str == "BEEP_ON" || str == "BEEP_OFF");
    if (str == "GREEN_LED")
    {
        msg.pop_front();
        if (!green_led_)
        {
            sock_.send("KO");
            return;
        }
        // forward the request to the led.
        green_led_->backend().send(msg);
        green_led_->backend().receive(str);
        assert(str == "OK" || str == "KO");
        sock_.send(str == "OK" ? "OK" : "KO");
    }
    else if (str == "BEEP")
    {
        assert (msg.parts() == 2);
        DEBUG("BEEP BEEP");
        std::string duration;
        msg >> duration;
        // our buzzer is a LED (this will change later tho)
        // BEEP:DURATION translates well to ON:DURATION for led.
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOn(std::chrono::milliseconds(std::stoi(duration)));
        assert(ret);
        sock_.send("OK");
    }
    else if (str == "BEEP_ON")
    {
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOn();
        assert(ret);
        sock_.send("OK");
    }
    else if (str == "BEEP_OFF")
    {
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOff();
        assert(ret);
        sock_.send("OK");
    }
}
