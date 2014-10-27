#pragma once

#include "modules/BaseModule.hpp"
#include "tools/log.hpp"
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
            enum Color
            {
                Black = 30,
                Red = 31,
                Green = 32,
                Yellow = 33,
                Blue = 34,
                Magenta = 35,
                Default
            };

            void handle_log_msg();

            void set_color(std::ostream &s, Color c);

            /**
            * Log message to stdout.
            */
            void log_stdout(const std::string &filename,
                    const std::string &funcname,
                    int line,
                    LogLevel level,
                    const std::string &message);

            /**
            * Log message to syslog
            */
            void log_syslog(const std::string &filename,
                    const std::string &funcname,
                    int line,
                    LogLevel level,
                    const std::string &message);

            /**
            * Receive log message on this socket.
            */
            zmqpp::socket sub_;
        };
    }
}
