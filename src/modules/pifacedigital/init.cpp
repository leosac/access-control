#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include "PFDigitalModule.hpp"

/**
* This function is the entry point of the PifaceDigital module.
* @see PFDigitalModule description.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    PFDigitalModule module(cfg, pipe, zmq_ctx);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    // this thread need realtime priority so it doesn't miss interrupt.
    struct sched_param p;
    p.sched_priority = 20;
    assert(pthread_setschedparam(pthread_self(), SCHED_RR, &p) == 0);

    module.run();

    std::cout << "module PFGpio shutying down" << std::endl;
    return true;
}