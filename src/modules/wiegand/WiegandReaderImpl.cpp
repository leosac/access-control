#include "WiegandReaderImpl.hpp"
#include "tools/log.hpp"

WiegandReaderImpl::WiegandReaderImpl(zmqpp::context &ctx,
        const std::string &data_high_pin,
        const std::string &data_low_pin) :
bus_sub_(ctx, zmqpp::socket_type::sub),
counter_(0)
{
    bus_sub_.connect("inproc://zmq-bus-pub");

    topic_high_ = "S_INT:" + data_high_pin;
    topic_low_ = "S_INT:" + data_low_pin;

    bus_sub_.subscribe(topic_high_);
    bus_sub_.subscribe(topic_low_);
}


WiegandReaderImpl::WiegandReaderImpl(WiegandReaderImpl &&o) :
bus_sub_(std::move(o.bus_sub_))
{
    topic_high_ = o.topic_high_;
    topic_low_ = o.topic_low_;

    buffer_ = o.buffer_;
    counter_ = o.counter_;
}


void WiegandReaderImpl::handle_bus_msg()
{
    std::string msg;
    bus_sub_.receive(msg);

    if (msg == topic_high_)
    {
        buffer_[counter_ / 8] |= (1 << (7 - counter_ % 8));
    }
    else if (msg == topic_low_)
    {

    }
    counter_++;
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_pin_activity_);
    last_pin_activity_ = std::chrono::system_clock::now();

    LOG() << "Someting to read for wiegand reader";
}

void WiegandReaderImpl::timeout()
{
    LOG() << "timeout, buffer size = " << counter_;
/*    std::size_t                 size = ((counter_ - 1) / 8) + 1;

    for (std::size_t i = 0; i < size; ++i)
        {
            c[i] = _buffer[size - i - 1];
            std::bitset<8> a(c[i]);
            LOG() << '[' << i << "] "<< a;
        }*/
    counter_ = 0;
}
