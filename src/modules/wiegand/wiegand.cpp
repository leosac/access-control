#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <fcntl.h>
#include "wiegand.hpp"
#include "zmqpp/actor.hpp"

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    WiegandReaderModule module(zmq_ctx, pipe, cfg);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    module.run();

    std::cout << "module WiegandReader shutting down." << std::endl;
    return true;
}

WiegandReaderModule::WiegandReaderModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        boost::property_tree::ptree const &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
        config_(cfg),
        is_running_(true)
{
    process_config();

    for (auto &reader : readers_)
    {
        reactor_.add(reader.bus_sub_, std::bind(&WiegandReaderImpl::handle_bus_msg, &reader));
    }
    reactor_.add(pipe_, std::bind(&WiegandReaderModule::handle_pipe, this));
}

void WiegandReaderModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("readers"))
    {
        boost::property_tree::ptree reader_cfg = node.second;

        std::string reader_name = reader_cfg.get_child("name").data();
        std::string gpio_high = reader_cfg.get_child("high").data();
        std::string gpio_low = reader_cfg.get_child("low").data();

        LOG() << "Creating READER " << reader_name;
        WiegandReaderImpl reader(ctx_, reader_name, gpio_high, gpio_low);
        readers_.push_back(std::move(reader));
    }
}

void WiegandReaderModule::handle_pipe()
{
    zmqpp::signal s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
}

void WiegandReaderModule::run()
{
    while (is_running_)
    {
        if (!reactor_.poll(10))
        {
            for (auto &reader : readers_)
                reader.timeout();
            //LOG() << "timeout";
        }

    }
}
