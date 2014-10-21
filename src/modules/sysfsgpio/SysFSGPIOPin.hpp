#pragma once
#include <zmqpp/zmqpp.hpp>
#include "SysFsGpioModule.hpp"

class SysFsGpioModule;
/**
* This is a implementation class. It's not exposed to the user and is for this
* module internal code only.
*
* It abstract a GPIO pin.
*/
class SysFsGpioPin
{
public:
    enum class Direction {
        In = 0,
        Out
    };

    SysFsGpioPin(zmqpp::context &ctx,
            const std::string &name,
            int gpio_no,
            Direction direction,
            SysFsGpioModule &module);

    ~SysFsGpioPin();

    SysFsGpioPin(const SysFsGpioPin &) = delete;

    SysFsGpioPin &operator=(const SysFsGpioPin &) = delete;
    SysFsGpioPin &operator=(SysFsGpioPin &&) = delete;

    SysFsGpioPin(SysFsGpioPin &&o) = delete;

    /**
    * Register own socket to the module's reactor.
    * @param reactor Reactor object owned by the module.
    */
    void register_sockets(zmqpp::reactor *reactor);

private:
    /**
    * Retrieve the underlying file descriptor of the GPIO.
    */
    int file_fd() const;

    /**
    * Interrupt happened for this GPIO ping.
    */
    void handle_interrupt();

    /**
    * Read value from filesystem.
    */
    bool read_value();

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
    * Write direction to the `direction` file.
    */
    void set_direction(const std::string &direction);

    /**
    * Write interrupt mode to the `edge` file.
    */
    void set_interrupt(const std::string &mode);

    /**
    * File descriptor of the GPIO in sysfs.
    */
    int file_fd_;

    /**
    * Number of the GPIO.
    */
    int gpio_no_;

    /**
    * listen to command from other component.
    */
    zmqpp::socket sock_;

    std::string name_;

    /**
    * Direction of the PIN.
    */
    const Direction direction_;

    /**
    * Reference to the module. We use this to publish on the bus.
    */
    SysFsGpioModule &module_;

};