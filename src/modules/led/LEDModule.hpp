#pragma once

#include "modules/BaseModule.hpp"
#include "LedImpl.hpp"
#include <memory>
#include <vector>

namespace Leosac
{
    namespace Module
    {
        /**
        * A module to support LED.
        *
        * While LED are basically a GPIO, this module is here to provide "higher level" construct to LED device, like BLINKing.
        * For simple command (`ON`, `OFF`, `TOGGLE`) it simply forwards them to the backend GPIO device that is linked with the
        * led, so it does not provide very useful addition.
        * However, For blinking (which requires multiple command, with varying delay) it's pretty useful.
        *
        * @see FLED documentation for LED devices API.
        */
        class LEDModule : public BaseModule
        {
        public:
            LEDModule(zmqpp::context &ctx,
                    zmqpp::socket *pipe,
                    const boost::property_tree::ptree &cfg);


            void run() override;

        private:
            /**
            * Compute timeout based on next_update_time of the LedImpl object.
            * Allow for handling BLINK easily.
            */
            int compute_timeout();


            void process_config();

            std::vector<std::shared_ptr<LedImpl>> leds_;
        };
    }
}