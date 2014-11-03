#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include "TestAndResetModule.hpp"

using namespace Leosac::Module::TestAndReset;

/**
* This function is the entry point of the Test And Reset module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    TestAndResetModule module(zmq_ctx, pipe, cfg);
    INFO("Module TestAndReset intialized");
    pipe->send(zmqpp::signal::ok);

    module.run();

    INFO("Module TestAndReset shutting down...");
    return true;
}