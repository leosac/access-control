#include <tools/log.hpp>
#include <fcntl.h>
#include <unistd.h>
#include "SysFSGPIOPin.hpp"

SysFsGpioPin::SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(ctx, zmqpp::socket_type::push),
        name_(name)
{
    LOG() << "trying to bind to " << ("inproc://" + name);
    sock_.bind("inproc://" + name);
    bus_push_.connect("inproc://zmq-bus-pull");
    std::string full_path = "/sys/class/gpio/gpio" + std::to_string(gpio_no) + "/value";
    LOG() << "PATH {" << full_path << "}";
    file_fd_ = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
    assert(file_fd_ != -1);
}

SysFsGpioPin::~SysFsGpioPin()
{
    if (file_fd_ != -1 && ::close(file_fd_) != 0)
    {
        LOG() << "fail to close fd " << file_fd_;
    }
    try
    {
        UnixFs::writeSysFsValue("/sys/class/gpio/unexport", gpio_no_);
    }
    catch (FsException &e)
    {
        LOG() << "Error while unexporting GPIO: " << e.what();
    }
}

SysFsGpioPin::SysFsGpioPin(SysFsGpioPin &&o) :
        sock_(std::move(o.sock_)),
        bus_push_(std::move(o.bus_push_))
{
    this->file_fd_ = o.file_fd_;
    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    o.file_fd_ = -1;
}

void SysFsGpioPin::set_direction(const std::string &direction)
{
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/direction", direction);
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
    // publish new state.
    LOG() << "gpio nammed {" << name_ << " will publish ";
    bus_push_.send(zmqpp::message() << ("S_" + name_) << (read_value() ? "ON" : "OFF"));
}

bool SysFsGpioPin::turn_on()
{
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/value", 1);
    return true;
}

bool SysFsGpioPin::turn_off()
{
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/value", 0);
    return true;
}

bool SysFsGpioPin::toggle()
{
    int v = UnixFs::readSysFsValue<int>("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/value");
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/value", v == 1 ? 0 : 1);
    return true;
}

bool SysFsGpioPin::read_value()
{
    return UnixFs::readSysFsValue<bool>("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/value");
}
