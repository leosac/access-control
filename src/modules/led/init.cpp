#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/zmqpp.hpp>
#include "LEDModule.hpp"

/**
* Entry point of the LED management module.
*/
extern "C" __attribute__((visibility("default")))
bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    LEDModule module(zmq_ctx, pipe, cfg);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    module.run();

    std::cout << "module LED shutting down." << std::endl;
    return true;
}