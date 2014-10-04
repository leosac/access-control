#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include "sysfsgpio.hpp"
#include "zmqpp/actor.hpp"

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg)
    {
    // assume custom module startup code.
    // when reeady, signal parent

    SysFsGpioModule module(pipe);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
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

SysFsGpioModule::SysFsGpioModule(zmqpp::socket *module_manager_pipe)
    {

    }
