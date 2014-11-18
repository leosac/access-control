#pragma once
#include "zmqpp/zmqpp.hpp"

/**
* Implements a message bus (running in its own thread) that use 2 sockets.
*
* PULL socket to receive message from client (available at `inproc://zmq-bus-pull`)
* PUB socket to publish everything it received (available at `inproc://zmq-bus-pub`)
*/
class MessageBus
    {
public:
    MessageBus(zmqpp::context &ctx);
    ~MessageBus();

private:
    zmqpp::actor *actor_;

    /**
    * The method that will be run in the child thread.
    * Pipe is a PAIR socket back to ZmqBus object.
    */
    bool run(zmqpp::socket *pipe);

    zmqpp::context &ctx_;

    zmqpp::socket *pub_;
    zmqpp::socket *pull_;


    void handle_pipe(zmqpp::socket *pipe);
    void handle_pull();

    bool running_;
    };