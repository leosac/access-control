/**
* \file init.cpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief init function for returning a module class to the core
*/

#include <zmqpp/zmqpp.hpp>
#include "RplethModule.hpp"
#include <tools/log.hpp>

using namespace Leosac::Module::Rpleth;

/**
* This function is the entry point of the Rpleth module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    {
        RplethModule module(zmq_ctx, pipe, cfg);

        INFO("Rpleth module initiliazed.");
        pipe->send(zmqpp::signal::ok);

        module.run();
        INFO("Rpleth module shutting down.");
    }
    INFO("Rpleth module terminated.");
    return true;
}
