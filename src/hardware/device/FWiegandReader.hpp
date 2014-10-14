#pragma once

#include <string>
#include <zmqpp/zmqpp.hpp>

/**
* Facade object for a Wiegand Reader device.
*
* You can control a reader through this class by acting on its led and buzzer.
*
* ### Wiegand Reader specifications:
* A wiegand reader device shall have 2 inputs GPIO configured and an optional greenled and buzzer
* devices.
*
* ### Command specifications:
* A wiegand device shall accept 2 commands:
*    + `BEEP` to activate the device for a given amount of time.
*    + `GREEN_LED` to execute a command on the associated led device.
*
* Command (Frame 1)        | Frame 2             | Frame 3          | Description
* -------------------------|---------------------|------------------|--------------
* BEEP                     | Duration (msec)     |                  | Make the buzzer beep for the given duration
* GREEN_LED                | A valid LED command | Parameters for command | Forward a LED command (and its parameters) to the green led
*
* @note A GREEN_LED command shall return a response as-if we sent a message to a LED device.
* @see FLED for a list of led command.
*/
class FWiegandReader
{
public:
    /**
    * Construct a facade to a wiegand reader; this facade will connect to the reader
    * @param ctx ZMQ context
    * @param reader_name name of targeted wiegand device
    */
    FWiegandReader(zmqpp::context &ctx, const std::string &reader_ame);

    FWiegandReader(const FWiegandReader &) = delete;
    FWiegandReader &operator=(const FWiegandReader &) = delete;

    ~FWiegandReader() = default;

    /**
    * Turn the reader's green led on.
    */
    bool greenLedOn();

    /**
    * Turn the reader's green led off.
    */
    bool greenLedOff();

    /**
    * Make the reader's green led blink.
    */
    bool greenLedBlink(int duration = 1000, int speed = 300);

private:

    /**
    * Send a message to the `backend_` wiegand reader and wait for a response.
    * The response shall be either "OK" or "KO"
    */
    bool send_to_backend(zmqpp::message &m);

    /**
    * A socket to talk to the backend wiegand reader.
    */
    zmqpp::socket backend_;
};
