#include <tools/log.hpp>
#include "AuthFileModule.hpp"

using namespace Leosac::Module::Auth;

/**
* Entry point for the auth-file module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    {
        AuthFileModule module(zmq_ctx, pipe, cfg);

        INFO("Module AuthFile initiaziled.");
        pipe->send(zmqpp::signal::ok);

        module.run();
    }
    INFO("Module AuthFile terminated.");
    return true;
}