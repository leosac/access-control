#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include "SysFSGPIOPin.hpp"

class SysFsGpioPin;

/**
* Handle GPIO management over sysfs.
* See below for configuration informations.
*
* ### Configuration Options
*
* Options | Options | Options        | Description                                            | Mandatory
* --------|---------|----------------|--------------------------------------------------------|-----------
* gpios   |         |                | List of GPIOs pins we configure                        | YES
* ----->  | gpio    |                | Configuration informations for one GPIO pin.           | YES
* ----->  | ----->  | name           | Name of the GPIO pin                                   | YES
* ----->  | ----->  | no             | Number of the GPIO pin.                                | YES
* ----->  | ----->  | direction      | Direction of the pin. This in either `in` or `out`     | YES
* ----->  | ----->  | interrupt_mode | What interrupt do we care about? See below for details | NO
* ----->  | ----->  | value          | Value of the PIN. Either `1` or `0`                    | NO
*
* #### Interrupt Mode
* `interrupt_mode` configuration option can take 4 values:
*     + `Falling`
*     + `Rising`
*     + `Both`
*     + `None`. This is the default.
*
* **This parameter is ignored for output GPIO.**
*
* #### Value
* `value` can be either `1` or `0` and is only meaningful for output GPIO. It default to `0`.
*/
class SysFsGpioModule
{
public:
    SysFsGpioModule(const boost::property_tree::ptree &config,
            zmqpp::socket *module_manager_pipe,
            zmqpp::context &ctx);

    ~SysFsGpioModule();

    /**
    * Module's main loop.
    */
    void run();

    /**
    * Write the message eon the bus.
    * This is intended for use by the SysFsGpioPin
    */
    void publish_on_bus(zmqpp::message &msg);

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
    * Write to "gpio_export_path" so the kernel export the socket to sysfs.
    */
    void export_gpio(int gpio_no);

    zmqpp::context &ctx_;

    /**
    * Socket to write the bus.
    */
    zmqpp::socket bus_push_;

    std::vector<SysFsGpioPin *> gpios_;
};
