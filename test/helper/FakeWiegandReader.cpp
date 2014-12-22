#include "FakeWiegandReader.hpp"

using namespace Leosac::Test::Helper;

FakeWiegandReader::FakeWiegandReader(zmqpp::context &ctx, const std::string &name) :
rep_(ctx, zmqpp::socket_type::rep)
{
    rep_.bind("inproc://" + name);
}

bool FakeWiegandReader::run(zmqpp::socket *pipe)
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
            zmqpp::message msg;
            rep_.receive(msg);
            rep_.send("OK");
        }
    }
    return true;
}