#include "AuthFileModule.hpp"

/**
* Entry point for the auth-file module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    {
        AuthFileModule module(zmq_ctx, pipe, cfg);

        std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
        pipe->send(zmqpp::signal::ok);

        module.run();
    }
    std::cout << "module AuthFile shutting down." << std::endl;
    return true;
}