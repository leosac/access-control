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

#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/version.hpp>
#include "exception/configexception.hpp"
#include "XmlPropertyTree.hpp"
#include "log.hpp"

using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::ptree_error;
using boost::property_tree::xml_parser::no_concat_text;
using boost::property_tree::xml_parser::no_comments;
using boost::property_tree::xml_parser::trim_whitespace;
using boost::property_tree::ptree;

boost::property_tree::ptree
Leosac::Tools::propertyTreeFromXmlFile(const std::string &path)
{
    boost::property_tree::ptree cfg;
    std::string filename(path);
    std::ifstream cfg_stream(filename);

    if (!cfg_stream.good())
        throw(ConfigException(filename, "Could not open file {" + path + "}"));
    try
    {
        read_xml(cfg_stream, cfg, trim_whitespace | no_comments);
        return cfg;
    }
    catch (ptree_error &e)
    {
        std::throw_with_nested(Ex::Config(filename));
    }
}

void Leosac::Tools::propertyTreeToXmlFile(const boost::property_tree::ptree &tree,
                                          const std::string &path)
{
    std::string filename(path);
    std::ofstream cfg_stream(filename);

    cfg_stream << propertyTreeToXml(tree);
}

std::string Leosac::Tools::propertyTreeToXml(const boost::property_tree::ptree &tree)
{
    std::stringstream ss;
    try
    {
#if ((BOOST_VERSION / 100000) == 1 && (BOOST_VERSION / 100 % 1000) >= 58) ||        \
    (BOOST_VERSION / 100000) > 1
        write_xml(
            ss, tree,
            boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
#else
        write_xml(ss, tree,
                  boost::property_tree::xml_writer_make_settings<char>('\t', 1));
#endif
    }
    catch (ptree_error &e)
    {
        std::throw_with_nested(std::runtime_error("Failed to serialize ptree"));
    }
    return ss.str();
}

bool Leosac::Tools::boost_text_archive_to_ptree(
    const std::string &data, boost::property_tree::ptree &tree) noexcept
{
    boost::property_tree::ptree cfg;
    std::istringstream iss(data);

    try
    {
        boost::archive::text_iarchive archive(iss);
        boost::property_tree::load(archive, cfg, 1);
        std::swap(cfg, tree);
        return true;
    }
    catch (std::exception &e)
    {
        ERROR(
            "Exception while extracting boost archive (as string) to property tree: "
            << e.what());
        return false;
    }
}
