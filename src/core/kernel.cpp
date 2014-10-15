#include <fstream>
#include "kernel.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <tools/log.hpp>
#include <tools/signalhandler.hpp>
#include <tools/unixshellscript.hpp>
#include "exception/configexception.hpp"

using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::ptree_error;
using boost::property_tree::xml_parser::no_concat_text;
using boost::property_tree::xml_parser::no_comments;
using boost::property_tree::xml_parser::trim_whitespace;
using boost::property_tree::ptree;

Kernel::Kernel(const boost::property_tree::ptree &config) :
        ctx_(),
        bus_(ctx_),
        control_(ctx_, zmqpp::socket_type::rep),
        config_(config),
        is_running_(true),
        want_restart_(false),
        module_manager_(ctx_)
{
    control_.bind("inproc://leosac-kernel");
}

boost::property_tree::ptree Kernel::make_config(const RuntimeOptions &opt)
{
    boost::property_tree::ptree cfg;
    std::string filename(opt.getParam("kernel-cfg"));
    std::ifstream cfg_stream(filename);

    if (!cfg_stream.good())
        throw (ConfigException(filename, "Could not open file"));
    try
    {
        read_xml(cfg_stream, cfg, trim_whitespace | no_comments);
        // store the path the config file.
        cfg.get_child("kernel").add("kernel-cfg", filename);
        return cfg.get_child("kernel"); // kernel is the root node.
    }
    catch (ptree_error &e)
    {
        throw (ConfigException(filename, std::string("Boost exception: ") + e.what()));
    }
}

bool Kernel::run()
{
    if (!module_manager_init())
        return false;

    SignalHandler::registerCallback(Signal::SigInt, [this](Signal)
    {
        this->is_running_ = false;
    });

    reactor_.add(control_, std::bind(&Kernel::handle_control_request, this));
    while (is_running_)
    {
        reactor_.poll(-1);
    }

    module_manager_.stopModules();
    return want_restart_;
}

bool Kernel::module_manager_init()
{
    try
    {
        ptree plugin_dirs = config_.get_child("plugin_directories");

        for (const auto &plugin_dir : plugin_dirs)
        {
            std::string pname = plugin_dir.first;
            std::string pvalue = plugin_dir.second.data();

            assert(pname == "plugindir");
            LOG() << "Adding {" << pvalue << "} in library path";
            module_manager_.addToPath(pvalue);
        }

        for (const auto &module : config_.get_child("modules"))
        {
            std::string pname = module.first;
            assert(pname == "module");

            ptree module_conf = module.second;
            std::string module_file = module_conf.get_child("file").data();

            if (!module_manager_.loadModule(module_conf))
                return false;
        }
    }
    catch (ptree_error &e)
    {
        LOG() << "Invalid configuration file: " << e.what();
        return false;
    }
    if (!module_manager_.initModules())
    {
        module_manager_.stopModules();
        return false;
    }
    return true;
}

Kernel::~Kernel()
{

}

void Kernel::handle_control_request()
{
    std::string req;

    control_.receive(req);
    LOG() << "Receive request: " << req;

    if (req == "RESTART")
    {
        is_running_ = false;
        want_restart_ = true;
    }
    if (req == "RESET")
    {
        is_running_ = false;
        want_restart_ = true;
        factory_reset();
    }
}

void Kernel::factory_reset()
{
    // we need to restore factory config file.
    UnixShellScript script("cp -f");

    std::string kernel_config_file = config_.get_child("kernel-cfg").data();
    LOG() << "Kernel config file path = " << kernel_config_file;
    LOG() << "RESTORING FACTORY CONFIG";

//    script.run(UnixShellScript::toCmdLine(rel_path_to_factory_conf + "/core.xml", _options.getParam("corecfg")));
//    script.run(UnixShellScript::toCmdLine(rel_path_to_factory_conf + "/hw.xml", _options.getParam("hwcfg")));
}
