#include <tools/log.hpp>
#include "MonitorModule.hpp"

using namespace Leosac::Module::Monitor;

MonitorModule::MonitorModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        bus_(ctx, zmqpp::socket_type::sub),
        verbose_(false)
{
    std::string system_bus_log_file = config_.get_child("module_config").get<std::string>("file-bus", "");
    if (!system_bus_log_file.empty())
    {
        reactor_.add(bus_, std::bind(&MonitorModule::log_system_bus, this));
        bus_.connect("inproc://zmq-bus-pub");
        bus_.subscribe("");
        spdlog::rotating_logger_mt("system_bus_event", "system_bus.log", 1024 * 1024 * 3, 2);
    }
    verbose_ = config_.get_child("module_config").get<bool>("verbose", false);
    if (verbose_)
    {
        spdlog::stdout_logger_mt("monitor_stdout");
    }
}

void MonitorModule::log_system_bus()
{
    auto system_bus_logger = spdlog::get("system_bus_event");
    assert(system_bus_logger);

    std::stringstream full_msg;
    zmqpp::message msg;
    bus_.receive(msg);

    for (size_t i = 0; i < msg.parts(); ++i)
    {
        std::string buf;
        msg >> buf;
        full_msg << "F" << i << ": {" << buf << "} ; ";
    }
    system_bus_logger->info(full_msg.str());
    if (verbose_)
    {
        auto monitor_stdout = spdlog::get("monitor_stdout");
        assert(monitor_stdout);
        monitor_stdout->info(full_msg.str());
    }
}
