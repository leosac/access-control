#include "pfdigital.hpp"
#include "pifacedigital.h"
#include "PFDigitalGPIO.hpp"
#include <cassert>
#include <tools/log.hpp>

void PFDigital::poll()
{
    int fd = pifacedigital_open(0);

    assert(pifacedigital_enable_interrupts() == 0);
    uint8_t ret = pifacedigital_wait_for_input(-1, 0);

    for (int i = 0 ; i < 8; ++i)
    {
        LOG() << "Input (" << i << ")  --> " << ((ret >> i) & 0x01);
    }

    pifacedigital_close(0);
}

GPIO *PFDigital::getGPIO(int gpioNo)
{
    LOG() << "Someone is asking a reference on PIN " << gpioNo;
    return new PFDigitalGPIO(gpioNo);
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

}

void PFDigital::poll_loop()
{
    while (run_)
    {
    LOG() << "Polling...";
        poll();
    }
}
