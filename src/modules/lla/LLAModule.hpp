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

#include "modules/BaseModule.hpp"
#include "modules/lla/LLAFwd.hpp"
#include "modules/lla/Worker.hpp"
#include <boost/property_tree/ptree.hpp>
#include <queue>
#include <zmqpp/reactor.hpp>
#include <zmqpp/socket.hpp>

namespace Leosac
{
namespace Module
{

namespace LLA
{
/**
 * A module that provide an Authentication source by
 * reader RFID card through liblogicalaccess.
 *
 * It is currently hardcoded to support PCSC readers.
 */
class LLAModule : public BaseModule
{
  public:
    using CardDescription = std::string;

    LLAModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
              const boost::property_tree::ptree &config, CoreUtilsPtr utils);

    ~LLAModule() = default;

    LLAModule(const LLAModule &) = delete;

    LLAModule &operator=(LLAModule &&) = delete;

    virtual void run() override;

  private:
    void process_config();

    /**
     * Used to spawn worker in run().
     */
    std::vector<WorkerConfiguration> worker_configs_;
};
}
}
}
