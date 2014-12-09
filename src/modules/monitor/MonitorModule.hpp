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
                        const boost::property_tree::ptree &cfg);
                
                MonitorModule(const MonitorModule &) = delete;
                MonitorModule(MonitorModule &&) = delete;
                MonitorModule &operator=(const MonitorModule &) = delete;
                MonitorModule &operator=(MonitorModule &&) = delete;


                virtual void run() override;

            private:
                using TimePoint = std::chrono::system_clock::time_point;

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

                std::unique_ptr<Leosac::Hardware::FLED> network_led_;

                TimePoint last_ping_;

                zmqpp::socket kernel_;
            };
        }
    }
}
