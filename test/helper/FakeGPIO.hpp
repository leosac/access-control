#pragma once
#include "zmqpp/zmqpp.hpp"

/**
* A test helper class that emulate a GPIO pin.
*/
class FakeGPIO
{
public:
    FakeGPIO(zmqpp::socket &push, const std::string &n) :
            name_(n),
            push_(push)
    {};

    /**
    * Simulate PIN interrupt: will write S_INT:{name} to the bus.
    */
    void interrupt()
    {
        push_.send("S_INT:" + name_);
    }

    std::string name_;
    zmqpp::socket &push_;
};
