/*
    Copyright (C) 2014-2015 Islog

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

#include "modules/BaseModule.hpp"

namespace Leosac
{
namespace Module
{
/**
* The instrumentation expose some internal of the program through IPC.
*
* It enable interesting testing features since it's goal is to emulate GPIO.
*/
namespace Instrumentation
{
class InstrumentationModule : public BaseModule
{
  public:
    InstrumentationModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                          const boost::property_tree::ptree &cfg,
                          CoreUtilsPtr utils);

    InstrumentationModule(const InstrumentationModule &) = delete;
    InstrumentationModule(InstrumentationModule &&)      = delete;
    InstrumentationModule &operator=(const InstrumentationModule &) = delete;
    InstrumentationModule &operator=(InstrumentationModule &&) = delete;

  private:
    void handle_command();

    void handle_gpio_command(zmqpp::message *str);

    zmqpp::socket bus_push_;

    /**
    * IPC ROUTER socket.
    */
    zmqpp::socket controller_;
};
}
}
}
