#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>

/**
* This function is the entry point of the Test And Reset module.
*
* This module watch preconfigured card and trigger factory RESET if needed.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    zmqpp::socket sock(zmq_ctx, zmqpp::socket_type::req);
    zmqpp::socket sub(zmq_ctx, zmqpp::socket_type::sub);

    sub.connect("inproc::/zmq-bus-pub");
    sub.subscribe("");

    sock.connect("inproc://leosac-kernel");
    pipe->send(zmqpp::signal::ok);

    zmqpp::poller p;
    p.add(sock, zmqpp::poller::poll_in);
    p.add(*pipe, zmqpp::poller::poll_in);

    while (true)
    {
        p.poll(-1);
        if (p.has_input(*pipe))
        {
            break;
        }
    }

    std::cout << "module test-and-reset shutting down" << std::endl;
    return true;
}