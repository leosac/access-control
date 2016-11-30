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

#include "LeosacFwd.hpp"
#include "Task.hpp"
#include "core/config/RemoteConfigCollector.hpp"

namespace Leosac
{
namespace Tasks
{
/**
 * Fetch the configuration from master server.
 *
 * This tasks should be scheduled in a pool thread.
 */
class FetchRemoteConfig : public Task
{
  public:
    FetchRemoteConfig(const std::string &endpoint, const std::string &pubkey);

    static constexpr const int timeout = 2000;

    const RemoteConfigCollector &collector() const;

  private:
    virtual bool do_run() override;

    zmqpp::context ctx_;
    RemoteConfigCollector collector_;
};
}
}
