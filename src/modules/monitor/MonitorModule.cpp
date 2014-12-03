#include <tools/log.hpp>
#include <tools/unixshellscript.hpp>
#include <zmqpp/z85.hpp>
#include "MonitorModule.hpp"

using namespace Leosac::Module::Monitor;

static std::string z85_pad(const std::string &in, char pad_char)
{
    std::string out(in);

    while (out.size() % 4)
        out += pad_char;
    return out;
}

MonitorModule::MonitorModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        bus_(ctx, zmqpp::socket_type::sub),
        verbose_(false),
        last_ping_(TimePoint::max())
{
    std::string system_bus_log_file = config_.get_child("module_config").get<std::string>("file-bus", "");
    if (!system_bus_log_file.empty())
    {
        reactor_.add(bus_, std::bind(&MonitorModule::log_system_bus, this));
        bus_.connect("inproc://zmq-bus-pub");
        bus_.subscribe("");
        spdlog::rotating_logger_mt("system_bus_event", system_bus_log_file, 1024 * 1024 * 3, 2);
    }
    verbose_ = config_.get_child("module_config").get<bool>("verbose", false);
    if (verbose_)
    {
        spdlog::stdout_logger_mt("monitor_stdout");
    }

    auto ping_node = config_.get_child("module_config").get_child_optional("ping");
    if (ping_node)
    {
        addr_to_ping_ = ping_node->get<std::string>("ip");
        std::string network_led_name = ping_node->get<std::string>("led");
        network_led_ = decltype (network_led_) (new Leosac::Hardware::FLED(ctx, network_led_name));
    }
}

void MonitorModule::run()
{
    while (is_running_)
    {
        reactor_.poll(1000);
        if (last_ping_ == TimePoint::max() ||
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_ping_).count() > 3)
        {
            test_ping();
            last_ping_ = std::chrono::system_clock::now();
        }
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
        if (std::find_if(buf.begin(), buf.end(), [](char c) { return !isprint(c);})
            != buf.end())
        {
            // encode frame in z85 since it contains non printable char
            buf = zmqpp::z85::encode(z85_pad(buf, 0));
            full_msg << "F" << i << ": {" << buf << "} ; ";
        }
        else
        {
            full_msg << "F" << i << ": {" << buf << "} ; ";
        }
    }
    system_bus_logger->info(full_msg.str());
    if (verbose_)
    {
        auto monitor_stdout = spdlog::get("monitor_stdout");
        assert(monitor_stdout);
        monitor_stdout->info(full_msg.str());
    }
}

void MonitorModule::test_ping()
{
    if (addr_to_ping_.empty())
        return;
    INFO("TESTING PING");
    Tools::UnixShellScript script("./scripts/ping.sh");

    int ret = script.run(addr_to_ping_);
    if (ret == 0)
    {
        INFO("PING OK");
        network_led_->turnOn();
    }
    else
    {
        WARN("Network looks down");
        network_led_->turnOff();
    }
}
