#pragma once

#include "zmqpp/zmqpp.hpp"
#include <boost/property_tree/ptree.hpp>

/**
* Base class for module implementation.
*
* It provide a base for writing module code. The base class
* implements `run()` as a main loop and use a reactor to handle socket
* when they become available for reading. When implementing a module using
* this class you're supposed to use that reactor and register your socket.
* In case you need to override `run()`, remember to poll on the reactor,
* or to watch the `pipe_` on way or another.*
*
* It use a reactor to poll on the `pipe` socket that connect the module back to the module manager.
*
* @note This class is here to help reduce code duplication. It is NOT mandatory to inherit from this base class
* to implement a module. However, it may help.
*/
class BaseModule
{
public:
    /**
    * Constructor of BaseModule. It will register the pipe_ to reactor_.
    */
    BaseModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

    virtual ~BaseModule() = default;

    /**
    * This is the main loop of the module. It should only exit when receiving `signal::stop` on its `pipe_`
    * socket. This is completely handled by this base class.
    * In case you override this, make sure you watch the `pipe_` socket.
    */
    virtual void run();

protected:
    /**
    * The base class register the `pipe_` socket to its `reactor_` so that this function
    * is called when the `pipe_` is available from reading.
    * This implementation switch `is_running_` to false.
    */
    virtual void handle_pipe();

    /**
    * A reference to the ZeroMQ context in case you need it to create additional socket.
    */
    zmqpp::context &ctx_;

    /**
    * A reference to the pair socket that link back to the module manager.
    */
    zmqpp::socket &pipe_;

    /**
    * The configuration tree passed to the `start_module` function.
    */
    boost::property_tree::ptree config_;

    /**
    * Boolean indicating whether the main loop should run or not.
    */
    bool is_running_;

    /**
    * The reactor object we poll() on in the main loop. Register additional socket/fd here if
    * you need to.
    */
    zmqpp::reactor reactor_;

};
