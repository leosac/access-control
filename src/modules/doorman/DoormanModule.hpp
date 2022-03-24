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

#include "core/auth/AuthTarget.hpp"
#include "hardware/facades/FGPIO.hpp"
#include "modules/BaseModule.hpp"
#include "tools/XmlScheduleLoader.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
/**
* Module that allows user to configure action to be taken
* to react to messages from other modules.
*
* @see @ref mod_doorman_main for end-user documentation.
*/
namespace Doorman
{

class DoormanInstance;

/**
* Main class for the module, it create handlers and run them
* to, well, handle events and send command.
*
* @see @ref mod_doorman_user_config for configuration information.
*/
class DoormanModule : public BaseModule
{
  public:
    DoormanModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                  const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    DoormanModule(const DoormanModule &) = delete;

    DoormanModule &operator=(const DoormanModule &) = delete;

    ~DoormanModule() = default;

    virtual void run() override;

    const std::vector<Auth::AuthTargetPtr> &doors() const;

  private:
    void update();

    /**
    * Processing the configuration tree, spawning AuthFileInstance object as
    * described in the
    * configuration file.
    */
    void process_config();

    void process_doors_config(const boost::property_tree::ptree &t);

    /**
    * Authenticator instances.
    */
    std::vector<std::shared_ptr<DoormanInstance>> doormen_;

    /**
    * Doors, to manage the always-on or always off stuff.
    */
    std::vector<Auth::AuthTargetPtr> doors_;
};
}
}
}
