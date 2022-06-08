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

#include "modules/AsioModule.hpp"
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace Alarms
{
class AlarmInstance;

/**
* This implements an alarm module that uses Leosac database
* to store the alarms
*/
class AlarmsModule : public BaseModule
{
  public:
    AlarmsModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                 const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    virtual ~AlarmsModule();

  private:
    void process_config();

    /**
     * Load the module configuration from the database.
     */
    void load_db_config();

    /**
     * Load the module configuration from the XML configuration
     * object.
     */
    void load_xml_config(const boost::property_tree::ptree &module_config);

    /**
    * Vector of alarms managed by this module.
    */
    std::vector<std::shared_ptr<AlarmInstance>> alarms_;
};
}
}
}
