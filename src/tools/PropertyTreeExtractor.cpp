/*
    Copyright (C) 2014-2016 Islog

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

#include "PropertyTreeExtractor.hpp"
#include "Colorize.hpp"
#include "exception/configexception.hpp"
#include "log.hpp"
#include <boost/property_tree/ptree.hpp>

using namespace Leosac::Tools;

PropertyTreeExtractor::PropertyTreeExtractor(const boost::property_tree::ptree &tree,
                                             const std::string &config_for)
    : tree_(tree)
    , text_(config_for)
{
}

template <typename T>
T PropertyTreeExtractor::get(const std::string &node_name)
{
    using namespace Colorize;
    try
    {
        return tree_.get<T>(node_name);
    }
    catch (const boost::property_tree::ptree_bad_path &)
    {
        Ex::Config e(text_, node_name, true);
        ERROR(e.what());
        std::throw_with_nested(e);
    }
    catch (const boost::property_tree::ptree_bad_data &)
    {
        Ex::Config e(text_, node_name, false);
        ERROR(e.what());
        std::throw_with_nested(e);
    }
}

template <typename T>
T PropertyTreeExtractor::get(const std::string &node_name, const T &default_value)
{
    return tree_.get<T>(node_name, default_value);
}

// Instantiate the template for a few basic types.

template std::string PropertyTreeExtractor::get<std::string>(const std::string &);
template int PropertyTreeExtractor::get<int>(const std::string &);
template uint64_t PropertyTreeExtractor::get<uint64_t>(const std::string &);

// Ditto, but for accessor with default value.

template std::string PropertyTreeExtractor::get<std::string>(const std::string &,
                                                             const std::string &);
template int PropertyTreeExtractor::get<int>(const std::string &, const int &);
template uint64_t PropertyTreeExtractor::get<uint64_t>(const std::string &,
                                                       const uint64_t &);
