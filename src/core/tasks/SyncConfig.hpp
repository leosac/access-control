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

#include "LeosacFwd.hpp"
#include "Task.hpp"

namespace Leosac
{
namespace Tasks
{
/**
 * Sync the configuration using the configuration fetched from the master
 * server.
 *
 * @note This tasks needs to run on the main thread.
 * @note The configuration must have been already fetched.
 */
class SyncConfig : public Task
{
  public:
    SyncConfig(Kernel &kref, FetchRemoteConfigPtr fetch_task,
               bool sync_general_config, bool autocommit);

  private:
    virtual bool do_run();
    void sync_config();

    Kernel &kernel_;
    /**
     * The task that fetch the data.
     */
    FetchRemoteConfigPtr fetch_task_;

    bool sync_general_config_;
    bool autocommit_;
};
}
}
