#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include "PFDigitalModule.hpp"

/**
* This function is the entry point of the PifaceDigital module.
* @see PFDigitalModule description.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    Leosac::Module::Piface::PFDigitalModule module(cfg, pipe, zmq_ctx);

    INFO("Module PFDigital initiliazed.");
    pipe->send(zmqpp::signal::ok);

    // this thread need realtime priority so it doesn't miss interrupt.
    struct sched_param p;
    p.sched_priority = 90;
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);
    assert(ret == 0);

    module.run();

    INFO("Module PFDigital shutting down.");
    return true;
}