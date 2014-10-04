#pragma once

#include <zmqpp/socket.hpp>
#include "modules/moduleconfig.h"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>

/**
* Handle GPIO management over sysfs.
*/
class SysFsGpioModule
    {
public:
    SysFsGpioModule(const boost::property_tree::ptree &config,
            zmqpp::socket *module_manager_pipe,
            zmqpp::context &ctx
    );

    /**
    * Module's main loop.
    */
    void run();

private:
    zmqpp::socket &pipe_;
    boost::property_tree::ptree config_;

    zmqpp::reactor reactor_;

    bool is_running_;

    /**
    * Handle message coming from the pipe.
    * This is basically handle the stop signal from the module manager.
    */
    void handle_pipe();

    //void handle_

    zmqpp::context &ctx_;
    };