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

#include <fstream>
#include "kernel.hpp"
#include "tools/log.hpp"
#include "tools/signalhandler.hpp"
#include "tools/unixshellscript.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/XmlPropertyTree.hpp"
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

using boost::property_tree::ptree;
using boost::property_tree::ptree_error;
using namespace Leosac::Tools;
using namespace Leosac;

Kernel::Kernel(const boost::property_tree::ptree &config) :
        config_manager_(config),
        ctx_(),
        bus_(ctx_),
        control_(ctx_, zmqpp::socket_type::rep),
        bus_push_(ctx_, zmqpp::socket_type::push),
        is_running_(true),
        want_restart_(false),
        module_manager_(ctx_, config_manager_),
        network_config_(nullptr),
        remote_controller_(nullptr),
        autosave_(false)
{
    configure_logger();
    extract_environ();

    if (config.get_child_optional("network"))
    {
        network_config_ = std::unique_ptr<NetworkConfig>(new NetworkConfig(*this, config.get_child("network")));
    }
    else
    {
        network_config_ = std::unique_ptr<NetworkConfig>(new NetworkConfig(*this, boost::property_tree::ptree()));
    }

    if (config.get_child_optional("remote"))
    {
        remote_controller_ = std::unique_ptr<RemoteControl>(new RemoteControl(ctx_, *this, config.get_child("remote")));
    }

    if (auto child = config.get_child_optional("autosave"))
    {
        autosave_ = (*child).get<bool>("");
    }

    control_.bind("inproc://leosac-kernel");
    bus_push_.connect("inproc://zmq-bus-pull");
    network_config_->reload();
}

boost::property_tree::ptree Kernel::make_config(const RuntimeOptions &opt)
{
    boost::property_tree::ptree cfg;
    std::string filename = opt.getParam("kernel-cfg");

    if (filename.empty())
        throw CoreException("Invalid command line parameter. No kernel configuration file specified.");

    try
    {
        cfg = propertyTreeFromXmlFile(filename);
        // store the path the config file.
        cfg.get_child("kernel").add("kernel-cfg", filename);
        return cfg.get_child("kernel"); // kernel is the root node.
    }
    catch (ptree_error &e)
    {
        std::throw_with_nested(ConfigException(filename, "Invalid main configuration"));
    }
}

bool Kernel::run()
{
    module_manager_init();

    SignalHandler::registerCallback(Signal::SigInt, [this](Signal)
    {
        this->is_running_ = false;
    });

    SignalHandler::registerCallback(Signal::SigTerm, [this](Signal)
    {
        this->is_running_ = false;
    });

    // At this point all module should have properly initialized.
    bus_push_.send(zmqpp::message() << "KERNEL" << "SYSTEM_READY");

    reactor_.add(control_, std::bind(&Kernel::handle_control_request, this));
    if (remote_controller_)
        reactor_.add(remote_controller_->socket_, std::bind(&RemoteControl::handle_msg, remote_controller_.get()));

    while (is_running_)
    {
        reactor_.poll(-1);
    }

    if (autosave_)
        save_config();
    return want_restart_;
}

void Kernel::module_manager_init()
{
    try
    {
        ptree plugin_dirs = config_manager_.kconfig().get_child("plugin_directories");

        for (const auto &plugin_dir : plugin_dirs)
        {
            std::string pname = plugin_dir.first;
            std::string pvalue = plugin_dir.second.data();

            assert(pname == "plugindir");
            DEBUG("Adding {" << pvalue << "} in library path");
            module_manager_.addToPath(pvalue);
        }

        for (const auto &module : config_manager_.kconfig().get_child("modules"))
        {
            std::string pname = module.first;
            assert(pname == "module");

            ptree module_conf = module.second;
            std::string module_file = module_conf.get_child("file").data();
            std::string module_name = module_conf.get_child("name").data();

            // we store the conf in our ConfigManager object, the ModuleManager will use it later.
            config_manager_.store_config(module_name, module_conf);

            if (!module_manager_.loadModule(module_conf))
                throw LEOSACException("Cannot load modules.");
        }
    }
    catch (ptree_error &e)
    {
        ERROR("Invalid configuration file: " << e.what());
        std::throw_with_nested(LEOSACException("Cannot load modules."));
    }
    module_manager_.initModules();
}

void Kernel::handle_control_request()
{
    zmqpp::message msg;
    std::string req;

    control_.receive(msg);
    msg >> req;
    INFO("Receive request: " << req);

    if (req == "RESTART")
    {
        is_running_ = false;
        want_restart_ = true;
        control_.send("OK");
    }
    else if (req == "RESET")
    {
        is_running_ = false;
        want_restart_ = true;
        factory_reset();
    }
    else if (req == "GET_NETCONFIG")
    {
        get_netconfig();
    }
    else if (req == "SET_NETCONFIG")
    {
        set_netconfig(&msg);
    }
    else if (req == "SCRIPTS_DIR")
    {
        control_.send(script_directory());
    }
    else if (req == "FACTORY_CONF_DIR")
    {
        control_.send(factory_config_directory());
    }
    else
    {
        ERROR("Unsupported message: " << req);
        assert(0);
    }
}

void Kernel::factory_reset()
{
    // we need to restore factory config file.
    UnixShellScript script("cp -f");

    std::string kernel_config_file = config_manager_.kconfig().get_child("kernel-cfg").data();
    INFO("Kernel config file path = " << kernel_config_file);
    INFO("RESTORING FACTORY CONFIG");

    if (script.run(UnixShellScript::toCmdLine(factory_config_directory() + "/kernel.xml",
            kernel_config_file)) != 0)
    {
        ERROR("Error restoring factory configuration...");
    }
}

void Kernel::get_netconfig()
{
    std::ostringstream oss;
    boost::archive::binary_oarchive archive(oss);
    auto network_config = config_manager_.kconfig().get_child("network");

    zmqpp::message response;
    boost::property_tree::save(archive, network_config, 1);
    response << oss.str();
    control_.send(response);
}

void Kernel::set_netconfig(zmqpp::message *msg)
{
    std::string serialized_config;
    *msg >> serialized_config;
    std::istringstream iss(serialized_config);
    boost::archive::binary_iarchive archive(iss);

    boost::property_tree::ptree network_config;
    boost::property_tree::load(archive, network_config, 1);

    config_manager_.kconfig().erase("network");
    config_manager_.kconfig().add_child("network", network_config);

    // we need to add the root node to write config;
    boost::property_tree::ptree to_save;

    to_save.add_child("kernel", config_manager_.kconfig());
    // remove path to config file.
    to_save.get_child("kernel").erase("kernel-cfg");
    try
    {
        Leosac::Tools::propertyTreeToXmlFile(to_save, config_manager_.kconfig().get_child("kernel-cfg").data());
    }
    catch (std::exception &e)
    {
        ERROR("Exception: " << e.what());
        control_.send("KO");
        return;
    }
    control_.send("OK");
}

void Kernel::extract_environ()
{
    if (char *str = getenv("LEOSAC_FACTORY_CONFIG_DIR"))
    {
        INFO("Using FACTORY_CONFIG_DIR: " << str);
        environ_[EnvironVar::FACTORY_CONFIG_DIR] = std::string(str);
    }
    if (char *str = getenv("LEOSAC_SCRIPTS_DIR"))
    {
        INFO("Using SCRIPTS_DIR: " << str);
        environ_[EnvironVar::SCRIPTS_DIR] = std::string(str);
    }
}

std::string Kernel::script_directory() const
{
    if (environ_.count(EnvironVar::SCRIPTS_DIR))
        return environ_.at(EnvironVar::SCRIPTS_DIR) + "/";
    return Leosac::Tools::UnixFs::getCWD() + "/scripts/";
}

std::string Kernel::factory_config_directory() const
{
    if (environ_.count(EnvironVar::FACTORY_CONFIG_DIR))
        return environ_.at(EnvironVar::FACTORY_CONFIG_DIR);
   return Leosac::Tools::UnixFs::getCWD() + "/share/leosac/cfg/factory/";
}

void Kernel::configure_logger()
{
    bool use_syslog                 = true;
    std::string syslog_min_level    = "WARNING";

    if (config_manager_.kconfig().get_child_optional("log"))
    {
        use_syslog          = config_manager_.kconfig().get_child("log").get<bool>("enable_syslog", true);
        syslog_min_level    = config_manager_.kconfig().get_child("log").get<std::string>("min_syslog", "WARNING");
    }
    if (use_syslog)
    {
        auto syslog = spdlog::create("syslog", {std::make_shared<spdlog::sinks::syslog_sink>()});
        syslog->set_level(static_cast<spdlog::level::level_enum>(LogHelper::log_level_from_string(syslog_min_level)));
    }
    auto console = spdlog::create("console", {std::make_shared<spdlog::sinks::stdout_sink_mt>()});
    console->set_level(spdlog::level::DEBUG);
}

const zModuleManager &Kernel::module_manager() const
{
    return module_manager_;
}

zModuleManager &Kernel::module_manager()
{
    return module_manager_;
}

bool Kernel::save_config()
{
    INFO("Saving current configuration to disk.");
    std::string full_config = Tools::propertyTreeToXml(config_manager_.get_application_config());
    std::string cfg_file_path = config_manager_.kconfig().get<std::string>("kernel-cfg");

    DEBUG("Will overwrite " << cfg_file_path << " in order to save configuration.");
    std::ofstream cfg_file(cfg_file_path);

    if (cfg_file << full_config)
        return true;
    return false;
}

zmqpp::context &Kernel::get_context()
{
    return ctx_;
}

ConfigManager &Kernel::config_manager()
{
    return config_manager_;
}

void Kernel::restart_later()
{
    want_restart_ = true;
    is_running_ = false;
}
