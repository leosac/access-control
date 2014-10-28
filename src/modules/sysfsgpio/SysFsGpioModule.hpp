#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include "SysFSGPIOPin.hpp"
#include "SysFsGpioConfig.hpp"

namespace Leosac
{
    namespace Module
    {
        /**
        * Namespace for the module that implements GPIO support using
        * the Linux Kernel sysfs interface.
        *
        * @see @ref mod_sysfsgpio_main for end-user documentation.
        */
        namespace SysFsGpio
        {
            class SysFsGpioPin;
            class SysFsGpioConfig;

            /**
            * Handle GPIO management over sysfs.
            * @see @ref mod_sysfsgpio_user_config for configuration information.
            */
            class SysFsGpioModule
            {
            public:
                SysFsGpioModule(const boost::property_tree::ptree &config,
                        zmqpp::socket *module_manager_pipe,
                        zmqpp::context &ctx);

                ~SysFsGpioModule();

                SysFsGpioModule(const SysFsGpioModule &) = delete;
                SysFsGpioModule &operator=(SysFsGpioModule &&) = delete;

                /**
                * Module's main loop.
                */
                void run();

                /**
                * Write the message eon the bus.
                * This is intended for use by the SysFsGpioPin
                */
                void publish_on_bus(zmqpp::message &msg);

                /**
                * Retrieve a reference to the config object.
                */
                const SysFsGpioConfig &general_config() const;

            private:
                zmqpp::socket &pipe_;
                boost::property_tree::ptree config_;

                zmqpp::reactor reactor_;

                bool is_running_;

                /**
                * Handle message coming from the pipe.
                * This is basically handle the stop signal from the module manager.
                */
                void handle_pipe();

                /**
                * Process the configuration, preparing configured GPIO pin.
                */
                void process_config(const boost::property_tree::ptree &cfg);

                /**
                * General configuration (file paths, etc).
                */
                bool process_general_config();

                /**
                * Write to "gpio_export_path" so the kernel export the socket to sysfs.
                */
                void export_gpio(int gpio_no);

                zmqpp::context &ctx_;

                /**
                * Socket to write the bus.
                */
                zmqpp::socket bus_push_;

                /**
                * Vector of underlying pin object
                */
                std::vector<SysFsGpioPin *> gpios_;

                /**
                * General configuration for module
                */
                SysFsGpioConfig *general_cfg_;
            };
        }
    }
}
