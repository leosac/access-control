#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include "SysFsGpioModule.hpp"

using namespace Leosac::Module::SysFsGpio;

/**
* Entry point of the SysFsGpio module.
* It provides a way to control GPIO through the sysfs kernel interface.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    try
    {
        SysFsGpioModule module(cfg, pipe, zmq_ctx);

        // this thread need realtime priority so it doesn't miss interrupt.
        struct sched_param p;
        p.sched_priority = 90;
        assert(pthread_setschedparam(pthread_self(), SCHED_FIFO, &p) == 0);
        INFO("Module SysFsGpio initiliazed.");

        pipe->send(zmqpp::signal::ok);

        module.run();
        INFO("Module SysFsGpio shutting down.");
    }
    catch (std::exception &e)
    {
        ERROR("Something bad happened: " << e.what());
        return false;
    }
    INFO("Module SysFsGpio terminated.");
    return true;
}
