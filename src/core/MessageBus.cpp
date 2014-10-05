#include "MessageBus.hpp"

MessageBus::MessageBus(zmqpp::context &ctx) :
ctx_(ctx),
running_(true)
    {
    actor_ = new zmqpp::actor(std::bind(&MessageBus::run, this, std::placeholders::_1));
    }

MessageBus::~MessageBus()
    {
    delete actor_;
    }

bool MessageBus::run(zmqpp::socket *pipe)
    {
    try
        {
        pub_ = new zmqpp::socket(ctx_, zmqpp::socket_type::pub);
        pub_->bind("inproc://zmq-bus-pub");

        pull_ = new zmqpp::socket(ctx_, zmqpp::socket_type::pull);
        pull_->bind("inproc://zmq-bus-pull");
        }
    catch (std::exception &e)
        {
        return false;
        }

    pipe->send(zmqpp::signal::ok);

    zmqpp::reactor reactor;

    reactor.add(*pull_, std::bind(&MessageBus::handle_pull, this));
    reactor.add(*pipe, std::bind(&MessageBus::handle_pipe, this, pipe));

    while (running_)
        {
        reactor.poll();
        }
    delete pull_;
    delete pub_;
    return true;
    }

void MessageBus::handle_pipe(zmqpp::socket *pipe)
    {
    zmqpp::signal  sig;
    pipe->receive(sig);

    if (sig == zmqpp::signal::stop)
        {
        running_ = false;
        }
    }

void MessageBus::handle_pull()
    {
    zmqpp::message msg;

    pull_->receive(msg);
    pub_->send(msg);
    }
