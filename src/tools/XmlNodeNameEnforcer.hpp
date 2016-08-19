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

#pragma once

#include <string>

namespace Leosac
{
namespace Tools
{
/**
 * This class is a simple wrapper that throws a ConfigException
 * message formated to report the user that a invalid xml node name
 * has been encountered.
 *
 * The class expect the path to the xml file, in order to pass it
 * to the ConfigException constructor.
 */
class XmlNodeNameEnforcer
{
  public:
    XmlNodeNameEnforcer(const std::string &path);

    /**
     * Check that actual is equal to expected, otherwise throws
     * a ConfigException with a meaningful message.
     */
    void enforce(const std::string &expected, const std::string &actual) const;

    /**
     * Forward the call to `enforce()`.
     */
    void operator()(const std::string &expected, const std::string &actual) const;

  private:
    std::string path_;
};
}
}
