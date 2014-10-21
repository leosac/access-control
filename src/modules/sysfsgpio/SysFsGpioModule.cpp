#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include "SysFsGpioModule.hpp"

SysFsGpioModule::SysFsGpioModule(const boost::property_tree::ptree &config,
        zmqpp::socket *module_manager_pipe,
        zmqpp::context &ctx) :
        pipe_(*module_manager_pipe),
        config_(config),
        is_running_(true),
        ctx_(ctx)
{
    process_config(config);

    for (auto &gpio : gpios_)
    {
        reactor_.add(gpio.sock_, std::bind(&SysFsGpioPin::handle_message, &gpio));
    }
    reactor_.add(pipe_, std::bind(&SysFsGpioModule::handle_pipe, this));
}

void SysFsGpioModule::run()
{
    while (is_running_)
    {
        reactor_.poll(-1);
    }
}

void SysFsGpioModule::handle_pipe()
{
    zmqpp::signal s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
}

void SysFsGpioModule::process_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;

        std::string gpio_name = gpio_cfg.get_child("name").data();
        int gpio_no = std::stoi(gpio_cfg.get_child("no").data());
        std::string gpio_direction = gpio_cfg.get_child("direction").data();

        LOG() << "Creating GPIO " << gpio_name << ", with no " << gpio_no << ". direction = " << gpio_direction;

        export_gpio(gpio_no);
        SysFsGpioPin pin(ctx_, gpio_name, gpio_no);

        pin.set_direction(gpio_direction);
        gpios_.push_back(std::move(pin));
    }
}

void SysFsGpioModule::export_gpio(int gpio_no)
{
    UnixFs::writeSysFsValue("/sys/class/gpio/export", gpio_no);
}
