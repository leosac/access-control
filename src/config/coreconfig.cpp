/**
 * \file coreconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief CoreConfig class implementation
 */

#include "coreconfig.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "exception/configexception.hpp"

using boost::property_tree::ptree;
using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser_error;

CoreConfig::CoreConfig(const std::string& path)
:   _path(path)
{}

void CoreConfig::load()
{
    std::ifstream   cfg(_path);
    ptree           pt;

    if (!cfg.good())
        throw (ConfigException(_path, "Could not open file"));
    try
    {
        read_xml(cfg, pt, boost::property_tree::xml_parser::trim_whitespace);
        BOOST_FOREACH(ptree::value_type const& v, pt.get_child("core"))
        {
            if (v.first == "plugin")
            {
                Plugin  plugin;

                plugin.file = v.second.get<std::string>("<xmlattr>.file", "default");
                plugin.alias = v.second.get<std::string>("alias");
                _plugins.push_back(plugin);
            }
            else if (v.first == "plugindir")
                _pluginDirs.push_back(v.second.data());
        }
    }
    catch (xml_parser_error& e)
    {
        throw (ConfigException(_path, e.what()));
    }
}

void CoreConfig::save()
{
    std::ofstream               cfg(_path);
    ptree                       pt;
    xml_writer_settings<char>   settings(' ', 4);

    if (!cfg.good())
        throw (ConfigException(_path, "Could not open file"));
    try
    {
        for (const auto& dir : _pluginDirs)
            ptree& node = pt.add("core.plugindir", dir);
        for (const auto& plugin : _plugins)
        {
            ptree& node = pt.add("core.plugin", "");

            node.put("<xmlattr>.file", plugin.file);
            node.put("alias", plugin.alias);
        }
        write_xml(cfg, pt, settings);
    }
    catch (const xml_parser_error& e)
    {
        throw (ConfigException(_path, e.what()));
    }
}

const std::list< std::string >& CoreConfig::getPluginDirs() const
{
    return (_pluginDirs);
}

const std::list< CoreConfig::Plugin >& CoreConfig::getPlugins() const
{
    return (_plugins);
}
