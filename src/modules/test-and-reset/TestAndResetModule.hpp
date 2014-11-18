#pragma once

#include <modules/BaseModule.hpp>
#include <hardware/device/FLED.hpp>

/**
* This is a module that watch input device and check for preconfigured reset/restart card.
*
* Options    | Options   | Options    | Description                                            | Mandatory
* -----------|-----------|------------|--------------------------------------------------------|-----------
* test_buzzer  |          |            | Device (beeper )to control when test card is read      | NO
* test_led  |             |            | Device (led) to control when test card is read        | NO
* devices    |           |            | List of devices to watch for                           | YES
* ---->      | device    |            | Watch for event sent by this device                    | YES
* ---->      | ---->     | name       | Name of the device                                     | YES
* ---->      | --->      | reset_card | Content / number of the card                           | NO
* ---->      | --->      | test_card  | Id of the card that trigger test led/beep stuff        | NO
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
    * Do some stuff to let the user known something happened.
    * This may be trigger on module initialization, and will be called when
    * test card is read.
    */
    void run_test_sequence();

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

    /**
    *  Map a device name to the test card.
    */
    std::map<std::string, std::string> device_test_card_;

    /**
    * Led device for test card
    */
    FLED *test_led_;

    /**
    * Buzzer device for test card
    */
    FLED *test_buzzer_;
};