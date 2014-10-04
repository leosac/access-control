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
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
zmqpp::context &zmq_ctx)
    {
    // assume custom module startup code.
    // when reeady, signal parent

    SysFsGpioModule module(cfg, pipe, zmq_ctx);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);


    module.run();

    std::cout << "module sysfsgpio shutying down" << std::endl;
    return true;
    }

SysFsGpioModule::SysFsGpioModule(const boost::property_tree::ptree &config,
        zmqpp::socket *module_manager_pipe,
zmqpp::context &ctx) :
pipe_(*module_manager_pipe),
config_(config),
is_running_(true),
ctx_(ctx)
    {
    reactor_.add(pipe_, std::bind(&SysFsGpioModule::handle_pipe, this));
    }

void SysFsGpioModule::run()
    {
    while (is_running_)
        {
        reactor_.poll(-1);
        }
    }

void SysFsGpioModule::handle_pipe()
    {
    zmqpp::signal  s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
    }
