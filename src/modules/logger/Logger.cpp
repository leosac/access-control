#include "Logger.hpp"

using namespace Leosac::Module::Logger;

LoggerSink::LoggerSink(zmqpp::context &ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        sub_(ctx, zmqpp::socket_type::sub)
{
    sub_.bind("inproc://log-sink");
    sub_.subscribe("");
    reactor_.add(sub_, std::bind(&LoggerSink::handle_log_msg, this));
}

void LoggerSink::handle_log_msg()
{
    zmqpp::message msg;
    std::string level;
    std::string content;

    sub_.receive(msg);
    msg >> level >> content;
    std::cout << "Message received YAY: " << level << " : " << content << std::endl;
}
