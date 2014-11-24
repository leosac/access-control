#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/zmqpp.hpp>
#include "LEDModule.hpp"

using namespace Leosac::Module::Led;

/**
* Entry point of the LED management module.
*/
extern "C" __attribute__((visibility("default")))
bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    LEDModule module(zmq_ctx, pipe, cfg);

    INFO("Module Led initialized.");
    pipe->send(zmqpp::signal::ok);

    module.run();
    INFO("Module Led shutting down");
    return true;
}