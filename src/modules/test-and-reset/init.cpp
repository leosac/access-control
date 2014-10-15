#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TestAndResetModule.hpp"

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
    TestAndResetModule module(zmq_ctx, pipe, cfg);

    pipe->send(zmqpp::signal::ok);

    module.run();

    std::cout << "module test-and-reset shutting down" << std::endl;
    return true;
}