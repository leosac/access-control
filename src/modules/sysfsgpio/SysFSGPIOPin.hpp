#pragma once
#include <zmqpp/zmqpp.hpp>

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