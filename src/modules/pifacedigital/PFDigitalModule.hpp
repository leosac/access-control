#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include "PFDigitalPin.hpp"

/**
* Application-level controller for the PiFaceDigital I/O extender cards: it provides support of
* GPIO object.
*
* It implements the *GPIO command set* described in the specifications of the facade object (FGPIO).
*
* Specifications of configuration informations:
*     1. A `<gpios>` entities defines all the GPIO you wish to export.
*     2. Each `<gpio>` entities define one GPIO pin.
*     3. `<name>` defines the application-name of the GPIO. The object shall be available through this name
*     and it shall use this name to publish message on the message bus.
*     4. `<no>` defines the number of the GPIO pin. It must be between 0 and 7.
*     5. `<direction>` defines whether this an INPUT or OUTPUT pin. It must be either `in` or `out`.
*     6. `<value>` defines the initial value. This is applicable only for OUTPUT pin.
*
* @note `<name>`, `<no>` and `<direction>` are mandatory configuration information.
* `<value>` is optional and default to `0`.
*
* @note Pin number if shared by input and outputs pin. Both use 0-7. This is important when writing configuration file.
* @see FGPIO for the GPIO command set specifications.
* @see PFDigitalPin for the helper class that is used by this module.
*/
class PFDigitalModule
{
public:
    PFDigitalModule(const boost::property_tree::ptree &config,
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

    /**
    * An interrupt was triggered. Lets handle it.
    */
    void handle_interrupt();

    /**
    * Process the configuration, preparing configured GPIO pin.
    */
    void process_config(const boost::property_tree::ptree &cfg);

    /**
    * A reference to our ZeroMQ context.
    */
    zmqpp::context &ctx_;

    /**
    * Socket to push event to the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * GPIO vector
    */
    std::vector <PFDigitalPin> gpios_;

    /**
    * Should be removed someday...
    * store the name of the input pin with id = idx in dest.
    *
    * returns true if it was succesful (pin exists), false otherwise.
    */
    bool get_input_pin_name(std::string &dest, int idx);

    /**
    * File descriptor of the PIN that triggers interrupts. This is card and will not change.
    */
    int interrupt_fd_;
};
