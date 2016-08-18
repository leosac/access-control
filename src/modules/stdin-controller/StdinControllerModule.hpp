#pragma once
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

#pragma once

#include "modules/BaseModule.hpp"

namespace Leosac
{
namespace Module
{
class StdinControllerModule : public BaseModule
{

  public:
    StdinControllerModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                          boost::property_tree::ptree const &cfg,
                          CoreUtilsPtr utils);

    /**
    * We can read from standard input;
    */
    void handleStdin(void);


    /**
    * Send the request to the target and handle the response.
    * Return false if no response in 1s.
    */
    bool send_request(std::shared_ptr<zmqpp::socket> target,
                      const std::vector<std::string> &cmds);

  private:
    // map a device name or anything that can be a target for a command to socket
    // that are connected to it.
    std::map<std::string, std::shared_ptr<zmqpp::socket>> endpoints_;
};
}
}