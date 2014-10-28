#include "Logger.hpp"

using namespace Leosac::Logger;

LoggerSink::LoggerSink(zmqpp::context &ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        sub_(ctx, zmqpp::socket_type::pull)
{
    sub_.bind("inproc://log-sink");
    reactor_.add(sub_, std::bind(&LoggerSink::handle_log_msg, this));
}

void LoggerSink::handle_log_msg()
{
    zmqpp::message msg;
    std::string filename;
    std::string funcname;
    int line;
    int lvl;
    LogLevel level;
    std::string content;

    sub_.receive(msg);
    msg >> filename >> funcname >> line >> lvl >> content;

    level = static_cast<LogLevel>(lvl);
    log_stdout(filename, funcname, line, level, content);
    log_syslog(filename, funcname, line, level, content);

}

void LoggerSink::set_color(std::ostream &s, LoggerSink::Color c)
{
    if (c == Default)
    {
        s << "\033[0m";
        return;
    }
    s << "\e[" << c << "m";
}

void LoggerSink::log_stdout(const std::string &filename,
        const std::string &funcname,
        int line,
        LogLevel level,
        const std::string &message)
{
    std::cout << "[";
    set_color(std::cout, Red);
    std::cout << LogHelper::log_level_to_string(level);
    set_color(std::cout, Default);
    std::cout << "] (" ;

    set_color(std::cout, Green);
    std::cout << filename;

    set_color(std::cout, Default);
    std::cout << ", " ;

    set_color(std::cout, Yellow);
    std::cout << funcname;
    set_color(std::cout, Default);

    std::cout << ":";
    set_color(std::cout, Magenta);
    std::cout << line;
    set_color(std::cout, Default);

    std::cout << ") --> " << std::endl << '\t';

    std::cout << message;
    set_color(std::cout, Default);
    std::cout << std::endl;
}

void LoggerSink::log_syslog(const std::string &filename,
        const std::string &funcname,
        int line,
        LogLevel level,
        const std::string &message)
{
    (void)filename;
    syslog(static_cast<int>(level), "(pid=%d) %s:%d -> %s", getpid(), funcname.c_str(), line, message.c_str());
}
