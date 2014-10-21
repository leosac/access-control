#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>

/**
* This is a implementation class. It's not exposed to the user and is for this
* module internal code only.
*
* It abstract a GPIO pin.
*/
struct SysFsGpioPin
{
    SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no);

    ~SysFsGpioPin();

    SysFsGpioPin(const SysFsGpioPin &) = delete;

    SysFsGpioPin &operator=(const SysFsGpioPin &) = delete;

    SysFsGpioPin(SysFsGpioPin &&o);


    /**
    * Read value from filesystem.
    */
    bool read_value();

    void set_direction(const std::string &direction);

    /**
    * Write to sysfs to turn the gpio on.
    */
    bool turn_on();

    /**
    * Write to sysfs to turn the gpio on.
    */
    bool turn_off();

    /**
    * Read to sysfs and then write the opposite value
    */
    bool toggle();

    /**
    * The SysFsGpioModule will register this method so its called when a message
    * is ready on the pin socket.
    */
    void handle_message();

    /**
    * File descriptor of the GPIO in sysfs.
    */
    int file_fd_;

    int gpio_no_;

    /**
    * listen to command from other component.
    */
    zmqpp::socket sock_;

    /**
    * PUSH socket to write to the bus.
    */
    zmqpp::socket bus_push_;

    std::string name_;
};

/**
* Handle GPIO management over sysfs.
*/
class SysFsGpioModule
{
public:
    SysFsGpioModule(const boost::property_tree::ptree &config,
            zmqpp::socket *module_manager_pipe,
            zmqpp::context &ctx);

    /**
    * Module's main loop.
    */
    void run();

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

    //void handle_

    /**
    * Process the configuration, preparing configured GPIO pin.
    */
    void process_config(const boost::property_tree::ptree &cfg);


    /**
    * Write to "gpio_export_path" so the kernel export the socket to sysfs.
    */
    void export_gpio(int gpio_no);

    zmqpp::context &ctx_;

    std::vector<SysFsGpioPin> gpios_;
};
