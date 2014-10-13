#include "FakeGPIO.hpp"

FakeGPIO::FakeGPIO(zmqpp::context &ctx,
        const std::string &name) :
ctx_(ctx),
name_(name),
push_(ctx, zmqpp::socket_type::push),
rep_(ctx, zmqpp::socket_type::rep),
value_(false)
{
    push_.connect("inproc://zmq-bus-pull");
    rep_.bind("inproc://" + name);
}

void FakeGPIO::interrupt()
{
    push_.send("S_INT:" + name_);
}

bool FakeGPIO::run(zmqpp::socket *pipe)
{
    zmqpp::poller poller;

    poller.add(*pipe);
    poller.add(rep_);

    pipe->send(zmqpp::signal::ok);

    while (true)
    {
        poller.poll(-1);

        if (poller.has_input(*pipe))
            break;

        if (poller.has_input(rep_))
        {
            handle_cmd();
        }
    }
    return true;
}

void FakeGPIO::handle_cmd()
{
    zmqpp::message_t msg;
    std::string frame1;
    rep_.receive(msg);

    msg >> frame1;
    if (frame1 == "ON")
        value_ = true;
    else if (frame1 == "OFF")
        value_ = false;
    else if (frame1 == "TOGGLE")
        value_ = !value_;
    else
        assert(0);

    push_.send(zmqpp::message() << ("S_" + name_) << (value_ ? "ON" : "OFF"));
    rep_.send("OK");
}
