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
    * REP socket to receive command on.
    */
    zmqpp::socket sock_;

    /**
    * Something happened on the bus.
    */
    void handle_bus_msg();

    /**
    * Someone sent a request.
    */
    void handle_request();

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
    * ZMQ topic-string for interrupt on HIGH gpio (high gpio's name)
    */
    std::string topic_high_;

    /**
    * ZMQ topic-string to interrupt on LOW gpio (low gpio's name)
    */
    std::string topic_low_;

    /**
    * Buffer to store incoming bits from high and low gpios.
    */
    std::array<uint8_t, 16> buffer_;

    /**
    * Count the number of bits received from GPIOs.
    */
    int counter_;

    /**
    * Name of the device (defined in configuration)
    */
    std::string name_;

    /**
    * Facade to control the reader green led.
    */
    FLED *green_led_;

    /**
    * Facade to the buzzer object, we use a LED device for now, because they are similar enough.
    */
    FLED *buzzer_;
};
