#pragma once

#include <vector>
#include <zmqpp/zmqpp.hpp>

struct DoormanAction
{
/**
* Target component. Will be reach through REQ.
*/
    std::string target_;

    /**
    * When should this action be done? on GRANTED or DENIED ?
    */
    std::string on_;

    /**
    * The command to be send. Each element of the vector will be a frame in the zmqpp::message.
    */
    std::vector <std::string> cmd_;
};

/**
* Implements a Doorman, that is, a component that will listen to authentication event and react accordingly.
* The reaction is somehow scriptable through the configuration file.
*/
class DoormanInstance
{
public:
    /**
    * Create a new doorman.
    *
    * @param ctx ZeroMQ context
    * @param name the name of this doorman
    * @param auth_contexts list of authentication context (by name) that we wish to watch
    * @param actions list of action to do when an event
    * @param timeout see timeout field description.
    */
    DoormanInstance(zmqpp::context &ctx,
            const std::string &name,
            const std::vector <std::string> &auth_contexts,
            const std::vector <DoormanAction> &actions,
            int timeout);

    DoormanInstance(const DoormanInstance &) = delete;
    DoormanInstance &operator=(const DoormanInstance &) = delete;

    zmqpp::socket &bus_sub();

    /**
    * Activity we care about happened on the bus.
    */
    void handle_bus_msg();

private:

    std::string name_;

    std::vector<DoormanAction> actions_;

    /**
    * This timeout value is used to determine if 2 auth event are accepted "as one".
    * Passed this delay, one event will not have any impact to the other.
    * //fixme rephrase that.
    */
    int timeout_;

    zmqpp::socket bus_sub_;
};
