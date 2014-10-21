#include <tools/log.hpp>
#include <fcntl.h>
#include <unistd.h>
#include "SysFSGPIOPin.hpp"

SysFsGpioPin::SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no,
        Direction direction,
        SysFsGpioModule &module) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        name_(name),
        direction_(direction),
        module_(module)
{
    LOG() << "trying to bind to " << ("inproc://" + name);
    sock_.bind("inproc://" + name);

    set_direction(direction_ == Direction::In ? "in" : "out");
    set_interrupt("rising");
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

void SysFsGpioPin::set_direction(const std::string &direction)
{
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/direction", direction);
}

void SysFsGpioPin::set_interrupt(const std::string &mode)
{
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/edge", mode);
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
    module_.publish_on_bus(zmqpp::message() << ("S_" + name_) << (read_value() ? "ON" : "OFF"));
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

int SysFsGpioPin::file_fd() const
{
    return file_fd_;
}

void SysFsGpioPin::handle_interrupt()
{
    std::array<char, 64> buffer;
    ssize_t ret;
    LOG() << "boap !";

    // flush interrupt by reading.
    // if we fail we cant recover, this means hardware failure.
    ret = ::read(file_fd_, &buffer[0], buffer.size());
    assert(ret >= 0);
    ret = ::lseek(file_fd_, 0, SEEK_SET);
    assert(ret >= 0);

    module_.publish_on_bus(zmqpp::message() << "S_INT:" + name_);
}

void SysFsGpioPin::register_sockets(zmqpp::reactor *reactor)
{
    reactor->add(sock_, std::bind(&SysFsGpioPin::handle_message, this));
    if (direction_ == Direction::In)
        reactor->add(file_fd_, std::bind(&SysFsGpioPin::handle_interrupt, this),
                zmqpp::poller::poll_pri);

}
