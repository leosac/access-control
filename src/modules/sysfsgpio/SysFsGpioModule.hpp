#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include <modules/BaseModule.hpp>
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
            class SysFsGpioModule : public BaseModule
            {
            public:
                SysFsGpioModule(const boost::property_tree::ptree &config,
                        zmqpp::socket *module_manager_pipe,
                        zmqpp::context &ctx);

                ~SysFsGpioModule();

                SysFsGpioModule(const SysFsGpioModule &) = delete;
                SysFsGpioModule &operator=(SysFsGpioModule &&) = delete;

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
