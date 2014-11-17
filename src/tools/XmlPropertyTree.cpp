#include <boost/property_tree/xml_parser.hpp>
#include "exception/configexception.hpp"
#include "XmlPropertyTree.hpp"

using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::ptree_error;
using boost::property_tree::xml_parser::no_concat_text;
using boost::property_tree::xml_parser::no_comments;
using boost::property_tree::xml_parser::trim_whitespace;
using boost::property_tree::ptree;

boost::property_tree::ptree Leosac::Tools::propertyTreeFromXmlFile(const std::string &path)
{
    boost::property_tree::ptree cfg;
    std::string filename(path);
    std::ifstream cfg_stream(filename);

    if (!cfg_stream.good())
        throw (ConfigException(filename, "Could not open file"));
    try
    {
        read_xml(cfg_stream, cfg, trim_whitespace | no_comments);
        return cfg;
    }
    catch (ptree_error &e)
    {
        throw (ConfigException(filename, std::string("Boost exception: ") + e.what()));
    }
}

void Leosac::Tools::propertyTreeToXmlFile(const boost::property_tree::ptree &tree,
        const std::string &path)
{
    std::string filename(path);
    std::ofstream cfg_stream(filename);

    if (!cfg_stream.good())
        throw (ConfigException(filename, "Could not open file"));
    try
    {
        boost::property_tree::xml_writer_settings<char> settings('\t', 1);
        write_xml(cfg_stream, tree, settings);
    }
    catch (ptree_error &e)
    {
        throw (ConfigException(filename, std::string("Boost exception: ") + e.what()));
    }
}
