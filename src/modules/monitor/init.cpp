#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <zmqpp/poller.hpp>
#include "zmqpp/actor.hpp"
#include "MonitorModule.hpp"

using namespace Leosac::Module::Monitor;

struct logger_guard
{

    logger_guard(const std::initializer_list<std::string> &s) :
            logger_names(s)
    {
    }

    ~logger_guard()
    {
        for (const auto &l : logger_names)
            spdlog::drop(l);
    }

    std::vector<std::string> logger_names;
};

/**
* This is the entry point of the Monitor module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    logger_guard g({"system_bus_event", "monitor_stdout"});
    {
        MonitorModule module(zmq_ctx, pipe, cfg);
        pipe->send(zmqpp::signal::ok);
        INFO("Module Monitor initiliazed.");

        module.run();

        INFO("Module Monitor shutting down.");
    }
    return true;
}
