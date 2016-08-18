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

#include "core/CoreUtils.hpp"
#include "modules/BaseModule.hpp"

namespace Leosac
{
namespace Module
{
/**
* This module handles the master/slave replication process, from the slave
* point of view.
*
* @see @ref mod_replication_main for documentation
*/
namespace Replication
{
/**
* Main class for the replication module.
*/
class ReplicationModule : public BaseModule
{
  public:
    ReplicationModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                      const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ReplicationModule(const ReplicationModule &) = delete;
    ReplicationModule(ReplicationModule &&)      = delete;
    ReplicationModule &operator=(const ReplicationModule &) = delete;
    ReplicationModule &operator=(ReplicationModule &&) = delete;

    virtual void run() override;

  private:
    using TimePoint = std::chrono::system_clock::time_point;
    void process_config();

    /**
     * Start the replication process.
     *
     * It first checks if it needs to sync, and if it doesn't, it stops
     * here.
     */
    void replicate();

    /**
     * Launch the tasks so that the synchronisation may take place.
     */
    void start_sync();

    /**
     * Fetch the local configuration version by running
     * a task in the main thread.
     */
    bool fetch_local_version(uint64_t &local);

    /**
     * Fetch the remote configuration version by running a task
     * in a pool, and sending the CONFIG_VERSION message.
     */
    bool fetch_remote_version(uint64_t &remote);

    /**
     * Delay between 2 replications attempt.
     */
    int delay_;

    /**
     * Target master server
     */
    std::string endpoint_;

    /**
     * Master server's public key.
     */
    std::string pubkey_;

    TimePoint last_sync_;
};
}
}
}
