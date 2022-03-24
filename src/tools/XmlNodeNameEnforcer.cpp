/*
    Copyright (C) 2014-2022 Leosac

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

#include "tools/XmlNodeNameEnforcer.hpp"
#include "exception/configexception.hpp"
#include "tools/Colorize.hpp"
#include <sstream>

using namespace Leosac;
using namespace Leosac::Tools;


XmlNodeNameEnforcer::XmlNodeNameEnforcer(const std::string &path)
    : path_(path)
{
}

void XmlNodeNameEnforcer::enforce(const std::string &expected,
                                  const std::string &actual) const
{
    if (actual != expected)
    {
        std::stringstream ss;
        ss << "Invalid configuration file content. Expected xml tag "
           << Colorize::green(expected) << " but found " << Colorize::green(actual)
           << " instead.";
        throw ConfigException(path_, ss.str());
    }
}

void XmlNodeNameEnforcer::operator()(const std::string &expected,
                                     const std::string &actual) const
{
    enforce(expected, actual);
}
