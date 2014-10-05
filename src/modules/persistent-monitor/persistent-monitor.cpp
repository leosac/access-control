#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <zmqpp/poller.hpp>
#include "zmqpp/actor.hpp"

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
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
            std::string buf;
            zmqpp::message msg;
            sub.receive(msg);

            LOG() << msg.parts() << "parts.";
            of << "New Entry: ";
            for (size_t i = 0; i < msg.parts(); ++i)
                {
                msg >> buf;
                LOG() << "part " << i << ": " << buf;
                of << buf;
                }
            of << std::endl;
            }
        }


    std::cout << "module sysfsgpio shutying down" << std::endl;
    return true;
    }
