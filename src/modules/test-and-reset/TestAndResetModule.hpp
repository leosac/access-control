#pragma once

#include <modules/BaseModule.hpp>

/**
* This is a module that watch input device and check for preconfigured reset/restart card.
*
* Options    | Options   | Options    | Description                                            | Mandatory
* -----------|-----------|------------|--------------------------------------------------------|-----------
* devices    |           |            | List of devices to watch for                           | YES
* ---->      | device    |            | Watch for event sent by this device                    | YES
* ---->      | ---->     | name       | Name of the device                                     | YES
* ---->      | --->      | reset_card | Content / number of the card                           | YES
*/
class TestAndResetModule : public BaseModule
{
public:
    TestAndResetModule(zmqpp::context &ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg);
    virtual ~TestAndResetModule();

private:
    void process_config();

    void handle_bus_msg();

    /**
    * REQ socket to kernel
    */
    zmqpp::socket kernel_sock_;

    /**
    * Sub socket on the BUS
    */
    zmqpp::socket sub_;

    /**
    * Map a device name to the reset card, since one device can support one reset card currently.
    */
    std::map<std::string, std::string> device_reset_card_;
};