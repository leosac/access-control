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
#include "hardware/FLED.hpp"

namespace Leosac
{
    namespace Module
    {
        /**
        * A module that provide visual feedback to the end-user about system's activity.
        *
        * @see @ref mod_monitor_main for documentation
        */
        namespace Monitor
        {
            /**
            * Main class for the monitor module.
            */
            class MonitorModule : public BaseModule
            {
            public:
                MonitorModule(zmqpp::context &ctx,
                              zmqpp::socket *pipe,
                              const boost::property_tree::ptree &cfg,
                              Scheduler &sched);

                MonitorModule(const MonitorModule &) = delete;

                MonitorModule(MonitorModule &&) = delete;

                MonitorModule &operator=(const MonitorModule &) = delete;

                MonitorModule &operator=(MonitorModule &&) = delete;

                virtual void run() override;

            private:
                using TimePoint = std::chrono::system_clock::time_point;

                void process_config();

                /**
                * Load config related to network monitoring
                */
                void process_network_config();

                /**
                * Load config related to reader activity monitoring.
                */
                void process_reader_config();

                /**
                * Get scripts directory from kernel.
                */
                std::string req_scripts_dir();

                /**
                * Called when a message arrives on the system bus and we
                * are configured to log that.
                */
                void log_system_bus();

                void test_ping();

                zmqpp::socket bus_;

                bool verbose_;

                std::string addr_to_ping_;

                std::string reader_to_watch_;

                /**
                * Led for feedback about network availability
                */
                std::unique_ptr<Leosac::Hardware::FLED> network_led_;

                /**
                * Led for feedback about reader activity
                */
                std::unique_ptr<Leosac::Hardware::FLED> reader_led_;

                /**
                * Led for feedback about system readiness
                */
                std::unique_ptr<Leosac::Hardware::FLED> system_led_;

                TimePoint last_ping_;

                zmqpp::socket kernel_;
            };
        }
    }
}
