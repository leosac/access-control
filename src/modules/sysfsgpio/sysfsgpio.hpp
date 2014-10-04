#pragma once

#include <zmqpp/socket.hpp>
#include "modules/moduleconfig.h"

/**
* Handle GPIO management over sysfs.
*/
class SysFsGpioModule
    {
public:
    SysFsGpioModule(zmqpp::socket *module_manager_pipe);

private:
    zmqpp::socket *pipe_;
    };