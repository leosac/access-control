#include <tools/log.hpp>
#include "DoormanModule.hpp"

using namespace Leosac::Module::Doorman;

/**
* Entry point for the Doorman module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    {
        DoormanModule module(zmq_ctx, pipe, cfg);

        INFO("Doorman Module initialized.");
        pipe->send(zmqpp::signal::ok);

        module.run();
    }
    INFO("Doorman module shutting down.");
    return true;
}