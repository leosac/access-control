#include "pfdigital.hpp"
#include "pifacedigital.h"
#include "PFDigitalGPIO.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <tools/log.hpp>

void PFDigital::poll()
{

  //  LOG() << "WILL WAIT_FOR_INPUT";
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    uint8_t ret = pifacedigital_wait_for_input(100, 0);
    // we cannot know if wait_for_input timeout'd or not.
    // we try to figure this out ourselve.
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (msElapsed >= 100)
    {
       // LOG() << "Timeout";
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
        std::cout << "Bit state: {";
        for (int i = 0; i < 8; ++i)
        {
            std::cout << (int) ((ret >> i) & 0x01);
            // LOG() << "Input (" << i << ")  --> " << ((ret >> i) & 0x01);
            for (auto &listener : listeners_[i])
            {
                if (((ret >> i) & 0x01) == 0)
                {
                    listener->notify(i);
                }
            }
        }
        std::cout << "}" << std::endl;
    }
    // process write order
    std::lock_guard<std::mutex> lock(order_queue_lock);
    while (order_queue.size())
    {
        std::pair<int, bool> p = order_queue.front();
        order_queue.pop();

        pifacedigital_write_bit(p.second ? 1 : 0, p.first, OUTPUT, 0);
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
      //  LOG() << "Polling...";
        poll();
    }
}

void PFDigital::set_pin_value(int pinNo, bool v)
{
    std::lock_guard<std::mutex> lock(order_queue_lock);

    order_queue.push(std::make_pair(pinNo, v));
}

PFDigital::~PFDigital()
{
    pifacedigital_close(0);
}
