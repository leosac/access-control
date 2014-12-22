/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
        last_ping_(TimePoint::max()),
        kernel_(ctx, zmqpp::socket_type::req)
{
    kernel_.connect("inproc://leosac-kernel");
    reactor_.add(bus_, std::bind(&MonitorModule::log_system_bus, this));
    bus_.connect("inproc://zmq-bus-pub");

    process_config();
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

    std::string src;
    std::stringstream full_msg;
    zmqpp::message msg;
    bus_.receive(msg);

    for (size_t i = 0; i < msg.parts(); ++i)
    {
        std::string buf;
        msg >> buf;
        if (i == 0)
        {
            src = buf;
        }
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

    // reader activity check
    if (src == ("S_" + reader_to_watch_) && reader_led_)
    {
        reader_led_->turnOn(500);
    }

    // system readiness check
    DEBUG("src = " << src);
    if (src == "KERNEL" && system_led_)
    {
        system_led_->turnOn();
    }
}

void MonitorModule::test_ping()
{
    if (addr_to_ping_.empty())
        return;
    INFO("TESTING PING");
    Tools::UnixShellScript script(req_scripts_dir() + "/ping.sh");

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

std::string MonitorModule::req_scripts_dir()
{
    std::string ret;
    kernel_.send("SCRIPTS_DIR");
    kernel_.receive(ret);
    return ret;
}

void MonitorModule::process_config()
{
    std::string system_bus_log_file = config_.get_child("module_config").get<std::string>("file-bus", "");
    if (!system_bus_log_file.empty())
    {
        bus_.subscribe("");
        spdlog::rotating_logger_mt("system_bus_event", system_bus_log_file, 1024 * 1024 * 3, 2);
    }
    verbose_ = config_.get_child("module_config").get<bool>("verbose", false);
    if (verbose_)
    {
        spdlog::stdout_logger_mt("monitor_stdout");
    }

    std::string system_led_name = config_.get_child("module_config").get<std::string>("system_ok", "");
    if (!system_led_name.empty())
    {
        system_led_ = std::unique_ptr<Leosac::Hardware::FLED>(new Leosac::Hardware::FLED(ctx_, system_led_name));
    }

    process_network_config();
    process_reader_config();
}

void MonitorModule::process_network_config()
{
    auto ping_node = config_.get_child("module_config").get_child_optional("ping");
    if (ping_node)
    {
        addr_to_ping_ = ping_node->get<std::string>("ip");
        std::string network_led_name = ping_node->get<std::string>("led");
        network_led_ = decltype (network_led_) (new Leosac::Hardware::FLED(ctx_, network_led_name));
    }
}

void MonitorModule::process_reader_config()
{
    auto reader_node = config_.get_child("module_config").get_child_optional("reader");
    if (reader_node)
    {
        reader_to_watch_ = reader_node->get<std::string>("name");
        bus_.subscribe("S_" + reader_to_watch_);
        std::string reader_led_name = reader_node->get<std::string>("led");
        reader_led_ = std::unique_ptr<Leosac::Hardware::FLED>(new Leosac::Hardware::FLED(ctx_, reader_led_name));
    }
}
