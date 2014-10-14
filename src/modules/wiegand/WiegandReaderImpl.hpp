#pragma once

#include <string>
#include <chrono>
#include "zmqpp/zmqpp.hpp"
#include "hardware/device/FLED.hpp"

/**
* An implementation class that represents a Wiegand Reader.
* It's solely for internal use by the Wiegand module.
*/
class WiegandReaderImpl
{
public:
    /**
    * Create a new implementation of a Wiegand Reader.
    * @param ctx ZMQ context.
    * @param data_high_pin name of the GPIO connected to data high.
    * @param data_low_pin name of the GPIO connected to data low.
    * @param green_led_name name of the "green led" LED device.
    * @param buzzer_name name of the buzzer device. -- no buzzer module yet.
    */
    WiegandReaderImpl(zmqpp::context &ctx,
            const std::string &name,
            const std::string &data_high_pin,
            const std::string &data_low_pin,
            const std::string &green_led_name,
            const std::string &buzzer_name);

    ~WiegandReaderImpl();

    WiegandReaderImpl(const WiegandReaderImpl &) = delete;
    WiegandReaderImpl &operator=(const WiegandReaderImpl &) = delete;

    WiegandReaderImpl(WiegandReaderImpl &&o);

    /**
    * Socket that allows the reader to listen to the application BUS.
    */
    zmqpp::socket bus_sub_;


    /**
    * Something happened on the bus.
    */
    void handle_bus_msg();

    /**
    * Timeout (no more data burst to handle). The WiegandModule call this when polling on any wiegand reader times out.
    * The reader shall publish an event if it received any meaningful message since the last timeout.
    */
    void timeout();
private:

    /**
    * Socket to write to the message bus.
    */
    zmqpp::socket bus_push_;

    /**
    * ZMQ topic-string to interrupt on HIGH gpio
    */
    std::string topic_high_;

    /**
    * ZMQ topic-string to interrupt on LOW gpio
    */
    std::string topic_low_;

    std::array<uint8_t, 16> buffer_;

    int counter_;

    /**
    * Name of the device (defined in configuration)
    */
    std::string name_;

    /**
    * Facade to control the reader green led.
    */
    FLED *green_led_;
};
