#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <fcntl.h>
#include "pifacedigital.hpp"
#include "zmqpp/actor.hpp"
#include "../../../libpifacedigital/src/pifacedigital.h"
#include "../../../libmcp23s17/src/mcp23s17.h"
#include "tools/unixsyscall.hpp"
#include "exception/gpioexception.hpp"
#include <pthread.h>

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
    PFGpioModule module(cfg, pipe, zmq_ctx);

    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    pipe->send(zmqpp::signal::ok);

    // this thread need realtime priority so it doesn't miss interrupt.
    struct sched_param p;
    p.sched_priority = 20;
    assert(pthread_setschedparam(pthread_self(), SCHED_RR, &p) == 0);

    module.run();

    std::cout << "module PFGpio shutying down" << std::endl;
    return true;
}

PFGpioModule::PFGpioModule(const boost::property_tree::ptree &config,
        zmqpp::socket *module_manager_pipe,
        zmqpp::context &ctx) :
        pipe_(*module_manager_pipe),
        config_(config),
        is_running_(true),
        ctx_(ctx),
        bus_push_(ctx_, zmqpp::socket_type::push)
{
    process_config(config);

    pifacedigital_open(0);

    bus_push_.connect("inproc://zmq-bus-pull");
    for (auto &gpio : gpios_)
    {
        reactor_.add(gpio.sock_, std::bind(&PFGpioPin::handle_message, &gpio));
    }
    reactor_.add(pipe_, std::bind(&PFGpioModule::handle_pipe, this));

    assert(pifacedigital_enable_interrupts() == 0);
    std::string path_to_gpio = "/sys/class/gpio/gpio" + std::to_string(GPIO_INTERRUPT_PIN) + "/value";
    interrupt_fd_ = open(path_to_gpio.c_str(), O_RDONLY | O_NONBLOCK);
    assert(interrupt_fd_ > 0);
    pifacedigital_read_reg(0x11, 0);//flush
    first_ = true; // Ignore GPIO Initial Event
    reactor_.add(interrupt_fd_, std::bind(&PFGpioModule::handle_interrupt, this), zmqpp::poller::poll_pri);
}

void PFGpioModule::run()
{
    while (is_running_)
    {
      reactor_.poll(-1);
    }
}

void PFGpioModule::handle_pipe()
{
    zmqpp::signal s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
}

void PFGpioModule::handle_interrupt()
{
    // get interrupt state.
    std::array<char, 64> buffer;

    if (::read(interrupt_fd_, &buffer[0], buffer.size()) < 0)
        throw (GpioException(UnixSyscall::getErrorString("read", errno)));
    if (::lseek(interrupt_fd_, 0, SEEK_SET) < 0)
        throw (GpioException(UnixSyscall::getErrorString("lseek", errno)));
    if (first_)
    {
        first_ = false;
        return;
    }
    uint8_t states = pifacedigital_read_reg(0x11, 0);
    for (int i = 0; i < 8; ++i)
    {
        if (((states >> i) & 0x01) == 0)
        {
            // this pin triggered interrupt
            //LOG() << "PIN " << i << " IS IN INTERRUPT MODE";
            //bus_push_.send(zmqpp::message() << "S_TEST" << (std::string("OMG INTERRUPT ON PIN " + std::to_string(i))));

            // signal interrupt if needed (ie the pin is registered in config
            std::string gpio_name;
            if (get_input_pin_name(gpio_name, i))
            {
                bus_push_.send(zmqpp::message() << std::string ("S_INT:" + gpio_name));
            }
        }
    }
  //  states = pifacedigital_read_reg(INPUT, 0);
 //       for (int i = 0; i < 8; ++i)
  //  {
   //    LOG() << "PIN " << i << " HAS VALUE: " << ((states >> i) & 0x01);
 //   }
   // pifacedigital_read_reg(0x11, 0); // flush
}


bool PFGpioModule::get_input_pin_name(std::string &dest, int idx)
{
    for (auto &gpio : gpios_)
    {
        if (gpio.gpio_no_ == idx && gpio.direction_ == PFGpioPin::Direction::In)
        {
            dest = gpio.name_;
            return true;
        }
    }
    return false;
}


void PFGpioModule::process_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;

        std::string gpio_name = gpio_cfg.get_child("name").data();
        int gpio_no = std::stoi(gpio_cfg.get_child("no").data());
        std::string gpio_direction = gpio_cfg.get_child("direction").data();

        LOG() << "Creating GPIO " << gpio_name << ", with no " << gpio_no;//<< ". direction = " << gpio_direction;

        //export_gpio(gpio_no);
        PFGpioPin pin(ctx_, gpio_name, gpio_no);

        if (gpio_direction != "in" && gpio_direction != "out")
            throw GpioException("Direction (" + gpio_direction + ") is invalid");
        pin.set_direction(gpio_direction == "in" ? PFGpioPin::Direction::In : PFGpioPin::Direction::Out);
        gpios_.push_back(std::move(pin));
    }
}

PFGpioPin::PFGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no) :
        gpio_no_(gpio_no),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(ctx, zmqpp::socket_type::push),
        name_(name)
{
    LOG() << "trying to bind to " << ("inproc://" + name);
    sock_.bind("inproc://" + name);
    bus_push_.connect("inproc://zmq-bus-pull");
}

PFGpioPin::~PFGpioPin()
{

}

PFGpioPin::PFGpioPin(PFGpioPin &&o) :
        sock_(std::move(o.sock_)),
        bus_push_(std::move(o.bus_push_))
{
    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    this->direction_ = o.direction_;
}

PFGpioPin &PFGpioPin::operator=(PFGpioPin &&o)
{
    sock_ = std::move(o.sock_);
    bus_push_ = std::move(o.bus_push_);

    this->gpio_no_ = o.gpio_no_;
    this->name_ = o.name_;
    this->direction_ = o.direction_;

    return *this;
}


void PFGpioPin::handle_message()
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
    else // invalid cmd
        ok = false;
    sock_.send(ok ? "OK" : "KO");

    // publish new state.
    bus_push_.send(zmqpp::message() << ("S_" + name_) << (read_value() ? "ON" : "OFF"));
}

bool PFGpioPin::turn_on()
{
    if (direction_ != Direction::Out)
        return false;
    pifacedigital_digital_write(gpio_no_, 1);
    return true;
}

bool PFGpioPin::turn_off()
{
   if (direction_ != Direction::Out)
        return false;
    pifacedigital_digital_write(gpio_no_, 0);
    return true;
}

bool PFGpioPin::toggle()
{
   if (direction_ != Direction::Out)
        return false;

    uint8_t v = pifacedigital_read_bit(gpio_no_, OUTPUT, 0);

    if (v)
        pifacedigital_digital_write(gpio_no_, 0);
    else
        pifacedigital_digital_write(gpio_no_, 1);
    return true;
}


void PFGpioPin::set_direction(PFGpioPin::Direction d)
{
    direction_ = d;
}

bool PFGpioPin::read_value()
{
    // pin's direction matter here (not read from same register).
    return pifacedigital_read_bit(gpio_no_, direction_ == Direction::Out ? OUTPUT : INPUT, 0);
}

