#include <memory>
#include <zmqpp/message.hpp>
#include "sysfsgpio.hpp"
#include "zmqpp/actor.hpp"

/**
* runs in new module thread
*/
/*
static bool __start_module(const ModuleConfig &cfg, zmqpp::socket *pipe)
    {
    try
        {
        SysFsGpioModule *module = new SysFsGpioModule(cfg, pipe);

        module->run();
        }
    catch (std::exception &e)
        {
        return false;
        }
    return  true;
    }
*/

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe)
    {
    // assume custom module startup code.
    // when reeady, signal parent

    std::cout << "Init ok... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    while (true)
        {
        // let do something stupid
        zmqpp::message m;
        zmqpp::signal s;

        pipe->receive(m, true);
        if (m.is_signal())
            {
            m >> s;
            if (s == zmqpp::signal::stop)
                break;
            }
        }


    std::cout << "module sysfsgpio shutying down" << std::endl;
    return true;
    }
