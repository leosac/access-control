#include <stdexcept>
#include <tools/log.hpp>
#include "gpio.hpp"
#include "PFDigitalGPIO.hpp"

PFDigitalGPIO::PFDigitalGPIO(int pinNo) :
        GPIO(pinNo)
{
    LOG() << "New piface digital GPIO";
}

bool PFDigitalGPIO::getValue() const
{
    throw std::runtime_error("GET_VALUE");
}

void PFDigitalGPIO::setValue(bool value) const
{
    throw std::runtime_error("SET_VALUE");
}

int PFDigitalGPIO::getPinNo() const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

const std::string &PFDigitalGPIO::getPath() const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

int PFDigitalGPIO::getPollFd() const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

GPIO::Direction PFDigitalGPIO::getDirection() const
{
     throw std::runtime_error("NOT_IMPLEMENTED");
}

void PFDigitalGPIO::setDirection(GPIO::Direction direction) const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

void PFDigitalGPIO::setValue(GPIO::Value value) const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

bool PFDigitalGPIO::isActiveLow() const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

void PFDigitalGPIO::setActiveLow(bool state) const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

bool PFDigitalGPIO::hasInterruptsSupport() const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}

GPIO::EdgeMode PFDigitalGPIO::getEdgeMode() const
{
     throw std::runtime_error("NOT_IMPLEMENTED");
}

void PFDigitalGPIO::setEdgeMode(GPIO::EdgeMode mode) const
{
    throw std::runtime_error("NOT_IMPLEMENTED");
}
