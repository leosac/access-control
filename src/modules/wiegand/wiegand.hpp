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

#include "zmqpp/zmqpp.hpp"
#include <boost/property_tree/ptree.hpp>
#include <modules/BaseModule.hpp>
#include "WiegandReaderImpl.hpp"

namespace Leosac
{
    namespace Module
    {

        /**
        * Provide support for Wiegand devices.
        *
        * @see @ref mod_wiegand_main for more information
        */
        namespace Wiegand
        {
            /**
            * This simply is the main class for the Wiegand module.
            */
            class WiegandReaderModule : public BaseModule
            {
            public:
                WiegandReaderModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);

                /**
                * Module's main loop. Will exit upon reception of signal::stop from module manager
                */
                virtual void run() override;

            private:
                /**
                * Create wiegand reader instances based on configuration.
                */
                void process_config();

                /**
                * Vector of wiegand reader managed by this module.
                */
                std::vector<WiegandReaderImpl> readers_;
            };
        }
    }
}
