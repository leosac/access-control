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
        ctx_(ctx),
        bus_push_(ctx_, zmqpp::socket_type::push)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    process_config(config);

    for (auto &gpio : gpios_)
    {
        gpio->register_sockets(&reactor_);
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
        std::string gpio_interrupt = gpio_cfg.get<std::string>("interrupt_mode", "none");
        bool gpio_initial_value = gpio_cfg.get<bool>("value", false);

        LOG() << "Creating GPIO " << gpio_name << ", with no " << gpio_no << ". direction = " << gpio_direction;

        export_gpio(gpio_no);
        SysFsGpioPin::Direction direction;
        SysFsGpioPin::InterruptMode interrupt_mode;

        if (gpio_interrupt == "none")
            interrupt_mode = SysFsGpioPin::InterruptMode::None;
        else if (gpio_interrupt == "both")
            interrupt_mode = SysFsGpioPin::InterruptMode::Both;
        else if (gpio_interrupt == "falling")
            interrupt_mode = SysFsGpioPin::InterruptMode::Falling;
        else if (gpio_interrupt == "rising")
            interrupt_mode = SysFsGpioPin::InterruptMode::Rising;

        direction = (gpio_direction == "in" ? SysFsGpioPin::Direction::In : SysFsGpioPin::Direction::Out);
        gpios_.push_back(new SysFsGpioPin(ctx_, gpio_name, gpio_no, direction, interrupt_mode,
                gpio_initial_value, *this));
    }
}

void SysFsGpioModule::export_gpio(int gpio_no)
{
    UnixFs::writeSysFsValue("/sys/class/gpio/export", gpio_no);
}

SysFsGpioModule::~SysFsGpioModule()
{
    for (auto gpio : gpios_)
        delete gpio;
}

void SysFsGpioModule::publish_on_bus(zmqpp::message &msg)
{
    bus_push_.send(msg);
}
