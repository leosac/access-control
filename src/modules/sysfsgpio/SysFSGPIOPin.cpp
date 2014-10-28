#include <tools/log.hpp>
#include <fcntl.h>
#include <unistd.h>
#include "SysFSGPIOPin.hpp"

using namespace Leosac::Module::SysFsGpio;

SysFsGpioPin::SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no,
        Direction direction,
        InterruptMode interrupt_mode,
        bool initial_value,
        SysFsGpioModule &module) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        name_(name),
        direction_(direction),
        initial_value_(initial_value),
        module_(module)
{
    LOG() << "trying to bind to " << ("inproc://" + name);
    sock_.bind("inproc://" + name);

    set_direction(direction);
    set_interrupt(interrupt_mode);
    std::string full_path = "/sys/class/gpio/gpio" + std::to_string(gpio_no) + "/value";

    if (direction == Direction::Out)
    {
        if (initial_value_)
            turn_on();
        else
            turn_off();
    }

    LOG() << "PATH {" << full_path << "}";
    file_fd_ = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
    assert(file_fd_ != -1);
}

SysFsGpioPin::~SysFsGpioPin()
{
    if (direction_ == Direction::Out)
    {
        if (initial_value_)
            turn_on();
        else
            turn_off();
    }

    if (file_fd_ != -1 && ::close(file_fd_) != 0)
    {
        LOG() << "fail to close fd " << file_fd_;
    }
    try
    {
        UnixFs::writeSysFsValue(module_.general_config().unexport_path(), gpio_no_);
    }
    catch (FsException &e)
    {
        LOG() << "Error while unexporting GPIO: " << e.what();
    }
}

void SysFsGpioPin::set_direction(Direction dir)
{
    std::string direction = dir == Direction::In ? "in" : "out";
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/direction", direction);
}

void SysFsGpioPin::set_interrupt(InterruptMode mode)
{
    std::string value;
    if (mode == SysFsGpioPin::InterruptMode::None)
        value = "none";
    else if (mode == SysFsGpioPin::InterruptMode::Both)
        value = "both";
    else if (mode == SysFsGpioPin::InterruptMode::Falling)
        value = "falling";
    else if (mode == SysFsGpioPin::InterruptMode::Rising)
        value = "rising";
    else
        assert (0);
    UnixFs::writeSysFsValue("/sys/class/gpio/gpio" + std::to_string(gpio_no_) + "/edge", value);
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

void SysFsGpioPin::handle_interrupt()
{
    std::array<char, 64> buffer;
    ssize_t ret;

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
