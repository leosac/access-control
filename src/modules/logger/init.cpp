#include <zmqpp/zmqpp.hpp>
#include "Logger.hpp"
#include <boost/property_tree/ptree.hpp>

using namespace Leosac::Module::Logger;

/**
* Entry point for the Logger module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    LoggerSink module(zmq_ctx, pipe, cfg);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    module.run();

    std::cout << "module logger shutting down" << std::endl;
    return true;
}