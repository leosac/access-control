/**
 * \file init.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief init function for returning a module class to the core
 */

#include <zmqpp/zmqpp.hpp>
#include "RplethModule.hpp"
#include <boost/property_tree/ptree.hpp>



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