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

#pragma once

#include "SysFSGPIOPin.hpp"
#include <boost/property_tree/ptree_fwd.hpp>
#include <map>
#include <string>

namespace Leosac
{
namespace Module
{

namespace SysFsGpio
{
/**
* Internal configuration helper for sysfsgpio module.
*
* This class provide a path resolution mechanism and aliases handling.
* It is implemented as an external object to ease testing.
*/
class SysFsGpioConfig
{
  public:
    /**
    * Construct the general config object
    * from the configuration tree.
    */
    SysFsGpioConfig(const boost::property_tree::ptree &cfg);

    /**
    * Returns the absolute path to the "export" sysfs file.
    */
    const std::string &export_path() const;

    /**
    * Returns the absolute path to the "unexport" sysfs file.
    */
    const std::string &unexport_path() const;

    /**
    * Compute the absolute path the "value" file for pin_no.
    */
    std::string value_path(int pin_no) const;

    /**
    * Compute the absolute path the "edge" file for pin_no.
    */
    std::string edge_path(int pin_no) const;

    /**
    * Compute the absolute path the "direction" file for pin_no.
    */
    std::string direction_path(int pin_no) const;

  private:
    /**
    * Maps pin number to file identifier.
    */
    std::map<int, std::string> pin_aliases_;

    /**
    * Default aliases rule, as defined in configuration
    */
    std::string default_aliases_;

    /**
    * Absolute path of the "export" sysfs file.
    */
    std::string cfg_export_path_;

    /**
    * Absolute path of the "unexport" sysfs file.
    */
    std::string cfg_unexport_path_;

    /**
    * Absolute path of the "value file" sysfs file: use `__PLACEHOLDER__` as a
    * placeholder
    * for the PIN identifier (likely its number, potentially -- prefixed by "gpio" on
    * raspberry pi).
    */
    std::string cfg_value_path_;

    /**
    * Absolute path to the "edge" file.
    */
    std::string cfg_edge_path_;

    /**
    * Absolute path to the "direction" file.
    */
    std::string cfg_direction_path_;
};
}
}
}