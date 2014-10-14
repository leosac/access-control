/**
 * \file init.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief init function for returning a module class to the core
 */

#include "rplethauth.hpp"
#include <zmqpp/zmqpp.hpp>
#include "RplethModule.hpp"
#include <boost/property_tree/ptree.hpp>

#ifndef MODULE_PUBLIC
#   ifdef rpleth_EXPORTS
#       define MODULE_PUBLIC __attribute__((visibility("default")))
#   elif defined(_WIN32)
#       define MODULE_PUBLIC
#   else
#       define MODULE_PUBLIC
#   endif
#endif

extern "C" MODULE_PUBLIC IModule* getNewModuleInstance(ICore& core, const std::string& name)
{
    return (new RplethAuth(core, name));
}

/**
* This function is the entry point of the Rpleth module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    RplethModule module(zmq_ctx, pipe, cfg);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    module.run();

    std::cout << "module Rpleth shutying down" << std::endl;
    return true;
}