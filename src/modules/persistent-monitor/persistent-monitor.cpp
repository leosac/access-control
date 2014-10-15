#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <zmqpp/poller.hpp>
#include "zmqpp/actor.hpp"

/**
* This is the entry point of the Persistent-Monitor module.
* This module is very simple: it logs everything that goes through the bus into a file.
* Optionally, this module can print to stdout.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    // assume custom module startup code.
    // when reeady, signal parent

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    // file we write event to.
    std::string file_name = cfg.get_child("module_config").get<std::string>("file");
    bool verbose = cfg.get_child("module_config").get<bool>("verbose", false);
    pipe->send(zmqpp::signal::ok);

    std::ofstream of(file_name);

    if (!of.good())
    {
        LOG() << "Invalid file";
        return false;
    }

    zmqpp::poller p;
    zmqpp::socket sub(zmq_ctx, zmqpp::socket_type::sub);

    sub.connect("inproc://zmq-bus-pub");
    sub.subscribe("");

    p.add(sub, zmqpp::poller::poll_in);
    p.add(*pipe, zmqpp::poller::poll_in);
    while (true)
    {
        p.poll(-1);

        if (p.has_input(*pipe))
        {
            break;
        }

        if (p.has_input(sub))
        {
            std::stringstream full_msg;
            zmqpp::message msg;
            sub.receive(msg);

            for (size_t i = 0; i < msg.parts(); ++i)
            {
                std::string buf;
                msg >> buf;
                full_msg << "F" << i << ": {" << buf << "} ; ";
            }
            if (verbose)
            {
                LOG() << full_msg.str();
            }
            full_msg << std::endl;
            of << full_msg.str();
        }
    }

    std::cout << "module Persistent-Monitor shutting down" << std::endl;
    return true;
}
