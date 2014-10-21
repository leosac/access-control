#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include "sysfsgpio.hpp"

/**
* Entry point of the SysFSGPIO module.
* It provides a way to control GPIO through the sysfs kernel interface.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    SysFsGpioModule module(cfg, pipe, zmq_ctx);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);
    module.run();
    std::cout << "module sysfsgpio shutying down" << std::endl;
    return true;
}
