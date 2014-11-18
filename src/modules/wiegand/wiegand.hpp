#pragma once

#include "zmqpp/zmqpp.hpp"
#include <boost/property_tree/ptree.hpp>
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
            class WiegandReaderModule
            {
            public:
                WiegandReaderModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);

                /**
                * Module's main loop. Will exit upon reception of signal::stop from module manager
                */
                void run();

                /**
                * Watch from stop signal from module manager
                */
                void handle_pipe();

            private:

                /**
                * Create wiegand reader instances based on configuration.
                */
                void process_config();

                zmqpp::reactor reactor_;

                /**
                * Vector of wiegand reader managed by this module.
                */
                std::vector<WiegandReaderImpl> readers_;

                zmqpp::context &ctx_;
                zmqpp::socket &pipe_;
                boost::property_tree::ptree config_;

                bool is_running_;
            };
        }
    }
}
