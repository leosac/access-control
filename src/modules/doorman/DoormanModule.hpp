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
#include "tools/db/db_fwd.hpp"
#include "tools/Schedule.hpp"
#include <boost/property_tree/ptree.hpp>
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
    /**
    * Processing the configuration tree, spawning AuthFileInstance object as
    * described in the
    * configuration file.
    */
    void process_config();

    void process_doors_config(const boost::property_tree::ptree &t);

    void process_db_schedules();

    /**
     * Adds open schedules to the doors from the database.
     */
    void add_open_door_schedules(odb::result<Tools::Schedule> &schedules, std::map<std::string, std::vector<Tools::SingleTimeFrame>> &door_open_timeframes);

    /**
     * Adds close schedules to the doors from the database.
     */
    void add_close_door_schedules(std::map<std::string, std::vector<Tools::SingleTimeFrame>> &door_open_timeframes);

    /**
     * Creates an inverse schedule based off of the open timeframes to close the door
     * when the open schedule is not active.
     */
    std::shared_ptr<Leosac::Tools::ISchedule> create_closed_schedule(const std::vector<Tools::SingleTimeFrame> &open_timeframes);
    
    /**
     * Creates a schedule with 7 timeframes (all days 00:00-23:59).
     */
    std::shared_ptr<Leosac::Tools::Schedule> create_24_7_schedule();

    /**
     * Adds the beginning of the day timeframe to a schedule.
     * Ex: If the open schedule is 08:00-17:00, the closed schedule will be 00:00-08:00.
     */
    void add_closed_tfs(std::vector<Tools::SingleTimeFrame> &open_tfs, std::shared_ptr<Leosac::Tools::Schedule> closed_schedule, int day);

    /**
     * Logs the open and closed timeframes for a door.
     */
    void log_open_and_closed_timeframes(const std::vector<Tools::SingleTimeFrame> &open_tfs, 
                                      const std::shared_ptr<Leosac::Tools::ISchedule> &closed_schedule, 
                                      const std::string &door_name);

    /**
     * Adds a timeframe to a schedule.
     */
    void add_timeframe_to_schedule(std::shared_ptr<Leosac::Tools::Schedule> schedule, int day, int start_hour, int start_min, int end_hour, int end_min);

    /**
     * Sorts a vector of timeframes by start time hh:mm.
     * Does not sort by day.
     */
    void sort_tf_vec_by_time(std::vector<Tools::SingleTimeFrame> &timeframes);

    /**
     * Adds and subtracts 1 minute from the start and end times of a timeframe.
     * Prevents open and close start/end times from being the same.
     * Ex: Open: 14:00-18:30 -> Closed: 14:01-18:29
     */
    void adjust_tf_times(int &start_hour, int &start_min, int &end_hour, int &end_min);

    /**
     * Sets the db_service_ member variable if use_db_schedules_ is true.
     */
    void set_db_service();

    /**
     * Clears always on and off schedules from all doors.
     */
    void clear_door_schedules();

    /**
     * Re-checks the database for updated/deleted schedules
     * Removes all schedules and re-adds them.
     */
    void refresh_db_schedules(std::chrono::system_clock::time_point now);

    /**
     * Checks if a schedule mapping contains only doors
     */
    bool is_door_schedule(const Tools::ScheduleMappingPtr &mapping);

    void update();

    /**
    * Authenticator instances.
    */
    std::vector<std::shared_ptr<DoormanInstance>> doormen_;

    /**
    * Doors, to manage the always-on or always off stuff.
    */
    std::vector<Auth::AuthTargetPtr> doors_;

    bool use_db_schedules_;

    DBServicePtr db_service_;

    std::chrono::system_clock::time_point last_schedule_refresh_;

    static constexpr std::chrono::seconds SCHEDULE_REFRESH_INTERVAL{60};
};
}
}
}
