#include <exception>
#include <exception/moduleexception.hpp>
#include "Logger.hpp"

using namespace Leosac::Logger;

LoggerSink::LoggerSink(zmqpp::context &ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        pull_(ctx, zmqpp::socket_type::pull),
        enable_syslog_(true),
        min_syslog_(LogLevel::WARN)
{
    try
    {
        process_config();
        pull_.bind("inproc://log-sink");
        reactor_.add(pull_, std::bind(&LoggerSink::handle_log_msg, this));
    }
    catch (...)
    {
        std::throw_with_nested(ModuleException("Logger init failed"));
    }
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

    pull_.receive(msg);
    msg >> filename >> funcname >> line >> lvl >> content;

    level = static_cast<LogLevel>(lvl);
    log_stdout(filename, funcname, line, level, content);

    // 0 is highest priority level
    if (enable_syslog_ && level <= min_syslog_)
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

void LoggerSink::process_config()
{
    std::string min_level;
    boost::property_tree::ptree log_cfg = config_.get_child("log");

    // Make sure LogLevel value are continous
    static_assert((LogLevel::DEBUG > LogLevel::INFO) && (LogLevel::INFO > LogLevel::NOTICE) &&
            (LogLevel::NOTICE > LogLevel::WARN) && (LogLevel::WARN > LogLevel::ERROR) &&
            (LogLevel::ERROR > LogLevel::ALERT) && (LogLevel::ALERT > LogLevel::EMERG), "Defined LogLevel value are not continous");

    enable_syslog_ = log_cfg.get<bool>("enable_syslog", true);
    min_level =  log_cfg.get<std::string>("min_syslog", "WARNING");
    min_syslog_ = LogHelper::log_level_from_string(min_level);
}
