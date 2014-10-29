#pragma once

#include <zmqpp/zmqpp.hpp>
#include "FakeGPIO.hpp"
#include <boost/property_tree/ptree.hpp>
#include "gtest/gtest.h"
#include "core/MessageBus.hpp"

extern thread_local zmqpp::socket *tl_log_socket;

/**
* Helper function that create an object of type ModuleType (using conventional parameter) and run it.
* This runs in "module"'s thread.
*/
template<typename ModuleType>
bool test_run_module(zmqpp::context *ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg)
{
    //create log socket for module thread
    tl_log_socket = new zmqpp::socket(*ctx, zmqpp::socket_type::push);
    tl_log_socket->connect("inproc://log-sink");

    {
        ModuleType module(*ctx, pipe, cfg);

        pipe->send(zmqpp::signal::ok);
        module.run();
    }

    delete tl_log_socket;
    return true;
}

/**
* Part of the `bus_read()` stuff.
*/
bool bus_read_extract(zmqpp::message *)
{
    return true;
}

/**
* Default frame extraction function.
*/
template<typename T, typename ...Content>
bool bus_read_extract(zmqpp::message *m, T first_arg, Content... content)
{
    T value;
    *m >> value;

    if (value != first_arg)
        return false;
    return bus_read_extract(m, content...);
}

/**
* Frame extraction method specialized (thanks to overloading) for `const char *`
*/
template<typename ...Content>
bool bus_read_extract(zmqpp::message *m, const char *first_arg, Content... content)
{
    std::string value;
    *m >> value;

    if (strcmp(value.c_str(), first_arg) != 0)
        return false;
    return bus_read_extract(m, content...);
}

/**
* Make a blocking read on the bus, return true if content match the message.
* false otherwise.
*/
template<typename ...Content>
bool bus_read(zmqpp::socket &sub, Content... content)
{
    zmqpp::message msg;

    if (!sub.receive(msg))
        return false;
    return bus_read_extract(&msg, content...);
}

/**
* Base class for test fixtures, it defines a ZMQ context, a BUS and a sub socket connect to the bus.
*/
class TestHelper : public ::testing::Test
{
private:
    /**
    * Called in the module's actor's thread.
    * Override this: Construct configuration for the module then `run_test_module()` with appropriate parameters.
    */
    virtual bool run_module(zmqpp::socket *pipe) = 0;

public:

    TestHelper() :
            ctx_(),
            bus_(ctx_),
            bus_sub_(ctx_, zmqpp::socket_type::sub),
            bus_push_(ctx_, zmqpp::socket_type::push),
            module_actor_(nullptr)
    {
        // create the log socket for main thread.
        tl_log_socket = new zmqpp::socket(ctx_, zmqpp::socket_type::push);
        tl_log_socket->connect("inproc://log-sink");

        bus_sub_.connect("inproc://zmq-bus-pub");
        bus_push_.connect("inproc://zmq-bus-pull");
    }

    virtual ~TestHelper()
    {
        delete module_actor_;
        delete tl_log_socket;
    }

    /**
    * We need this 2-step initialization to prevent calling virtual method (run_module) in constructor.
    */
    virtual void SetUp() override final
    {
        module_actor_ = new zmqpp::actor(std::bind(&TestHelper::run_module, this, std::placeholders::_1));
    }

    /**
    * A context for the test case
    */
    zmqpp::context ctx_;

    /**
    * A BUS identical to the one spawned by core.
    */
    MessageBus bus_;

    /**
    * A SUB socket connected to the previous bus.
    */
    zmqpp::socket bus_sub_;


    /**
    * A PUSH socket to write on the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * An actor, to run the module code the same way it would be run by the core.
    * Does NOT subscribe to anything.
    */
    zmqpp::actor *module_actor_;
};
