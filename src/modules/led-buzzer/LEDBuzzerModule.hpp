/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "modules/BaseModule.hpp"
#include "LedBuzzerImpl.hpp"
#include <memory>
#include <vector>

namespace Leosac
{
    namespace Module
    {
        /**
        * Namespace where implementation of Led (or buzzer) support is done.
        *
        * While LED are basically a GPIO, this module is here to provide "higher level" construct to LED device, like BLINKing.
        * For simple command (`ON`, `OFF`, `TOGGLE`) it simply forwards them to the backend GPIO device that is linked with the
        * led, so it does not provide very useful addition.
        * However, For blinking (which requires multiple command, with varying delay) it's pretty useful.
        *
        * Since logical buzzer (that you can toggle on or off) work in the same way, this modules
        * handles both.
        *
        * @see @ref mod_ledbuzzer_main for end-user documentation.
        */
        namespace LedBuzzer
        {
            /**
            * Main class for LED support.
            *
            * @see FLED documentation for LED devices API.
            * @see @ref mod_led_user_config for end-user documentation.
            */
            class LEDBuzzerModule : public BaseModule
            {
            public:
                LEDBuzzerModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg,
                        CoreUtilsPtr  utils);

                virtual ~LEDBuzzerModule() = default;

                LEDBuzzerModule(const LEDBuzzerModule &) = delete;
                LEDBuzzerModule(LEDBuzzerModule &&) = delete;
                LEDBuzzerModule &operator=(const LEDBuzzerModule &) = delete;
                LEDBuzzerModule &operator=(LEDBuzzerModule &&) = delete;

                void run() override;

            private:
                void process_config();

                std::vector<std::shared_ptr<LedBuzzerImpl>> leds_;
            };
        }
    }
}
