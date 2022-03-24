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

#pragma once

#include "leosacexception.hpp"
#include "tools/Colorize.hpp"
#include <string>

namespace Leosac
{
namespace Ex
{
/**
 * An exception related to configuration.
 */
class Config : public LEOSACException
{
  public:
    /**
     * Construct a config exception for when we failed to load a property.
     *
     * @param config_target The subsystem that tried to use / load the
     *                      configuration.
     * @param config_entry The key (property) that is the source of the
     *                      exception.
     * @param not_found Is the exception due to not finding the key, or
     *                  invalid entry.
     */
    Config(const std::string &config_target, const std::string &config_entry,
           bool not_found)
        : LEOSACException(build_message(config_target, config_entry, not_found))
    {
    }

    /**
     * Create a config exception when we failed to parse a
     * configuration file.
     */
    Config(const std::string &filename)
        : LEOSACException(build_message(filename))
    {
    }

  private:
    static std::string build_message(const std::string &config_target,
                                     const std::string &config_entry, bool not_found)
    {
        using namespace Colorize;
        if (not_found)
        {
            return "Missing configuration entry for " + green(config_target) + ": " +
                   underline(red(config_entry));
        }
        else
        {
            return "Invalid configuration entry for " + green(config_target) + ": " +
                   underline(red(config_entry));
        }
    }

    static std::string build_message(const std::string &filename)
    {
        using namespace Colorize;
        return "Failed to parse configuration file " + underline(green(filename));
    }
};
}
}

class ConfigException : public LEOSACException
{
  public:
    ConfigException(const std::string &file, const std::string &message)
        : LEOSACException("Configuration error in file {" + file + "}: " + message)
    {
    }
    virtual ~ConfigException()
    {
    }
};
