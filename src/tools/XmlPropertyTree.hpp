/*
    Copyright (C) 2014-2016 Leosac

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

#include <boost/property_tree/ptree_fwd.hpp>

#pragma once

namespace Leosac
{
namespace Tools
{
/**
* Build a property tree from a xml file.
* @param path Path to the xml file.
*/
boost::property_tree::ptree propertyTreeFromXmlFile(const std::string &path);

/**
* Write a property tree to an xml file.
*/
void propertyTreeToXmlFile(const boost::property_tree::ptree &tree,
                           const std::string &path);

/**
* Convert a property tree to an xml formatted string.
*/
std::string propertyTreeToXml(const boost::property_tree::ptree &tree);

/**
* Convert a boost text archive, whose content is represented as a string (data) to
* a property tree.
*
* If this failed for any reason, return false.
* Otherwise returns true.
* This function does not throw.
*/
bool boost_text_archive_to_ptree(const std::string &data,
                                 boost::property_tree::ptree &tree) noexcept;
}
}
