/**
 * \file xmlconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief XmlConfig class implementation
 */

#include "xmlconfig.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "exception/configexception.hpp"

using boost::property_tree::ptree;
using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::ptree_error;
using boost::property_tree::xml_parser::no_concat_text;
using boost::property_tree::xml_parser::no_comments;
using boost::property_tree::xml_parser::trim_whitespace;

XmlConfig::XmlConfig(const std::string& file, IXmlSerializable& object)
:   _file(file),
    _object(object)
{}

void XmlConfig::serialize()
{
    std::ofstream               cfg(_file);
    ptree                       pt;
    xml_writer_settings<char>   settings(' ', WhiteSpaceIndent);

    if (!cfg.good())
        throw (ConfigException(_file, "Could not open file"));
    try
    {
        _object.serialize(pt);
        write_xml(cfg, pt, settings);
    }
    catch (const ptree_error& e)
    {
        throw (ConfigException(_file, e.what()));
    }
}

void XmlConfig::deserialize()
{
    std::ifstream   cfg(_file);
    ptree           pt;

    if (!cfg.good())
        throw (ConfigException(_file, "Could not open file"));
    try
    {
        read_xml(cfg, pt, trim_whitespace | no_comments);
        _object.deserialize(pt);
    }
    catch (ptree_error& e)
    {
        throw (ConfigException(_file, e.what()));
    }
}
