#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <fcntl.h>
#include "sysfsgpio.hpp"
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
    // assume custom module startup code.
    // when reeady, signal parent

    SysFsGpioModule module(cfg, pipe, zmq_ctx);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);


    module.run();

    std::cout << "module sysfsgpio shutying down" << std::endl;
    return true;
    }

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
    zmqpp::signal  s;

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

SysFsGpioPin::SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no) :
sock_(ctx, zmqpp::socket_type::rep)
    {
    sock_.bind("inproc://" + name);
    std::string full_path = "/sys/class/gpio" + std::to_string(gpio_no);
    file_fd_ = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
    assert(file_fd_ != -1);
    }

SysFsGpioPin::~SysFsGpioPin()
    {
    assert(::close(file_fd_));
    }

void SysFsGpioModule::export_gpio(int gpio_no)
    {
    UnixFs::writeSysFsValue("/sys/class/gpio/export", gpio_no);
    }

SysFsGpioPin::SysFsGpioPin(SysFsGpioPin &&o) :
sock_(std::move(o.sock_))
    {
    this->file_fd_ = o.file_fd_;
    }

void SysFsGpioPin::set_direction(const std::string &direction)
    {
    UnixFs::writeSysFsValue("/sys/class/gpio" + std::to_string(gpio_no_) + "/direction", direction);
    }

void SysFsGpioPin::handle_message()
    {
    zmqpp::message_t msg;
    std::string frame1;
    sock_.receive(msg);

    msg >> frame1;
    bool ok = false;
    if (frame1 == "ON")
        ok = turn_on();
    else if (frame1 == "OFF")
        ok = turn_off();
    else if (frame1 == "TOGGLE")
        ok = toggle();
    sock_.send(ok ? "OK" : "KO");
    }

bool SysFsGpioPin::turn_on()
    {
    UnixFs::writeSysFsValue("/sys/class/gpio" + std::to_string(gpio_no_) + "/value", 1);
    return true;
    }

bool SysFsGpioPin::turn_off()
    {
    UnixFs::writeSysFsValue("/sys/class/gpio" + std::to_string(gpio_no_) + "/value", 0);
    return true;
    }

bool SysFsGpioPin::toggle()
    {
    int v = UnixFs::readSysFsValue<int>("/sys/class/gpio" + std::to_string(gpio_no_) + "/value");
    UnixFs::writeSysFsValue("/sys/class/gpio" + std::to_string(gpio_no_) + "/value", v == 1 ? 0 : 1);
    return true;
    }
