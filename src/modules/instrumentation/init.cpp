#include <tools/log.hpp>
#include "InstrumentationModule.hpp"

using namespace Leosac::Module::Instrumentation;

extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    {
        InstrumentationModule module(zmq_ctx, pipe, cfg);

        INFO("Instrumentation module initialized.");
        pipe->send(zmqpp::signal::ok);

        module.run();
    }
    INFO("Instrumentation module shutting down.");
    return true;
}
