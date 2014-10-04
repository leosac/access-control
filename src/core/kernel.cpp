#include <fstream>
#include "kernel.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <tools/log.hpp>
#include <tools/signalhandler.hpp>
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
config_(config),
is_running_(true)
    {

    }

boost::property_tree::ptree Kernel::make_config(const RuntimeOptions &opt)
    {
    boost::property_tree::ptree cfg;
    std::string filename(opt.getParam("kernel-cfg"));
    std::ifstream   cfg_stream(filename);

    if (!cfg_stream.good())
        throw (ConfigException(filename, "Could not open file"));
    try {
        read_xml(cfg_stream, cfg, trim_whitespace | no_comments);
        return cfg.get_child("kernel"); // kernel is the root node.
        }
    catch (ptree_error& e) {
        throw (ConfigException(filename, std::string("Boost exception: ") + e.what()));
        }
    }

bool Kernel::run()
    {

    if (!module_manager_init())
        return false;

    SignalHandler::registerCallback(Signal::SigInt, [this] (Signal s) { this->is_running_ = false ;});

    while (is_running_);

    module_manager_.stopModules();
    return false ; // we dont want to restart;
    }

bool Kernel::module_manager_init()
    {
        ptree plugin_dirs = config_.get_child("plugin_directories");

    for (const auto & plugin_dir : plugin_dirs)
        {
        std::string pname = plugin_dir.first;
        std::string pvalue = plugin_dir.second.data();

        assert(pname == "plugindir");
        LOG() << "Adding {" << pvalue << "} in library path";
        module_manager_.addToPath(pvalue);
        }

    for (const auto & module : config_.get_child("modules"))
        {
        std::string pname = module.first;
        assert(pname == "module");

        ptree module_conf = module.second;
        std::string module_file = module_conf.get_child("file").data();

        module_manager_.loadModule(module_conf);
        }

    if (!module_manager_.initModules())
        {
        module_manager_.stopModules();
        return false;
        }
    return true;
    }
