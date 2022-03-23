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

#include "LibgpiodPin.hpp"
#include <boost/property_tree/ptree_fwd.hpp>
#include <map>
#include <string>

namespace Leosac
{
namespace Module
{

namespace Libgpiod
{
/**
* Internal configuration helper for libgpiod module.
*/
class LibgpiodConfig
{
  public:
    /**
    * Construct the general config object
    * from the configuration tree.
    */
    LibgpiodConfig(const boost::property_tree::ptree &cfg);

    /**
    * Returns the consumer name that will be temporary assigned to the GPIO.
    */
    const std::string &consumer() const;

  private:

    /**
    * The consumer name that will be temporary assigned to the GPIO
    */
    std::string consumer_;
};
}
}
}
