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

#include "PFDigitalPin.hpp"
#include "modules/BaseModule.hpp"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include <zmqpp/socket.hpp>

namespace Leosac
{
namespace Module
{
/**
* Provide support for the piface digital device.
*
* @see @ref mod_piface_main for documentation
*/
namespace Piface
{
/**
* Main class for the piface digital module.
*/
class PFDigitalModule : public BaseModule
{
  public:
    PFDigitalModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                    const boost::property_tree::ptree &config, CoreUtilsPtr utils);

    /**
    * Module's main loop.
    */
    virtual void run() override;

  private:
    /**
    * An interrupt was triggered. Lets handle it.
    */
    void handle_interrupt();

    /**
    * Process the configuration, preparing configured GPIO pin.
    */
    void process_config(const boost::property_tree::ptree &cfg);

    /**
    * Socket to push event to the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * GPIO vector
    */
    std::vector<PFDigitalPin> gpios_;

    /**
    * Should be removed someday...
    * store the name of the input pin with id = idx in dest.
    *
    * returns true if it was succesful (pin exists), false otherwise.
    */
    bool get_input_pin_name(std::string &dest, int idx);

    /**
    * File descriptor of the PIN that triggers interrupts. This is card and will not
    * change.
    */
    int interrupt_fd_;
};
}
}
}
