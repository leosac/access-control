#include "pfdigital.hpp"
#include "pifacedigital.h"
#include "PFDigitalGPIO.hpp"
#include <cassert>
#include <iostream>
#include <tools/log.hpp>

void PFDigital::poll()
{
    uint8_t states;
    bool timeout = (pifacedigital_wait_for_input2(&states, 100, 0) > 0 ? false : true);

    if (timeout)
    {
        LOG() << "Timeout";
        for (int i = 0; i < 8; ++i)
        {
            for (auto &listener : listeners_[i])
            {
                listener->timeout();
            }
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            for (auto &listener : listeners_[i])
            {
                if (((states >> i) & 0x01) == 0)
                {
                    listener->notify(i);
                }
            }
        }
    }
}

GPIO *PFDigital::getGPIO(int gpioNo)
{
    LOG() << "Someone is asking a reference on PIN " << gpioNo;
    return new PFDigitalGPIO(gpioNo, this);
}

void PFDigital::registerListener(IGPIOListener *listener, GPIO *gpio)
{
    listeners_[gpio->getPinNo()].push_back(listener);
}

void PFDigital::unregisterListener(IGPIOListener *listener, GPIO *gpio)
{
    listeners_[gpio->getPinNo()].remove(listener);
}

void PFDigital::start_poll()
{
    run_ = true;
    _pollThread = std::thread([this] () { poll_loop(); } );
}

void PFDigital::stop_poll()
{
    run_ = false;
    _pollThread.join();
}

PFDigital::PFDigital()
{
    piface_fd_ = pifacedigital_open(0);
    assert(pifacedigital_enable_interrupts() == 0);
}

void PFDigital::poll_loop()
{
    while (run_)
    {
        poll();
        flush_write();
    }
}

void PFDigital::set_pin_value(int pinNo, bool v)
{
    std::lock_guard<std::mutex> lock(order_queue_lock);

    order_queue.push(std::make_pair(pinNo, v));
}

PFDigital::~PFDigital()
{
    LOG() << "Shutting down PF digital GPIO manager.";
    pifacedigital_close(0);
}

void PFDigital::flush_write()
{
    std::lock_guard<std::mutex> lock(order_queue_lock);
    while (order_queue.size())
    {
        LOG() << "WRITING TO GPIO";
        std::pair<int, bool> p = order_queue.front();
        order_queue.pop();
        assert(p.first >= 8 && p.first <= 15); // OUTPUT pin start at 8.

        pifacedigital_write_bit(p.second ? 1 : 0, p.first - 8, OUTPUT, 0);
    }
}
