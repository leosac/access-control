#pragma once

#include "modules/BaseModule.hpp"
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
    /**
    * This module implements Logging capability into Leosac.
    * This works by creating a SUB socket and bind it to a known endpoint.
    *
    * The logging macro (see log.hpp) will use thread_local variable to access publisher
    * that will write to this subscriber.
    *
    * This gives us a nice, thread-safe and flexible logging framework.
    *
    * @note This module is automatically started by the core.
    */
    namespace Logger
    {
        /**
        * The LoggerSink collect all message from the other components
        * of the application and handles them based on (yet-to-be) defined rules.
        */
        class LoggerSink : public BaseModule
        {
        public:
            LoggerSink(zmqpp::context &ctx,
                    zmqpp::socket *pipe,
                    const boost::property_tree::ptree &cfg);

        private:
            void handle_log_msg();

            /**
            * Receive log message on this socket.
            */
            zmqpp::socket sub_;
        };
    }
}
