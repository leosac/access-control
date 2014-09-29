#include <stdexcept>
#include <cassert>
#include <tools/log.hpp>
#include "gpio.hpp"
#include "PFDigitalGPIO.hpp"

PFDigitalGPIO::PFDigitalGPIO(int pinNo, PFDigital *provider) :
        GPIO(pinNo),
pfdigital_provider_(provider)
{
    LOG() << "New piface digital GPIO";
}

bool PFDigitalGPIO::getValue() const
{
    throw std::runtime_error("GET_VALUE");
}

void PFDigitalGPIO::setValue(bool value) const
{
    setValue(value ? GPIO::Value::High : GPIO::Value::Low);

    //throw std::runtime_error("SET_VALUE");
}

int PFDigitalGPIO::getPinNo() const
{
    return _pinNo;
}

const std::string &PFDigitalGPIO::getPath() const
{
    throw std::runtime_error("NOT_IMPLEMENTED: getPath");
}

int PFDigitalGPIO::getPollFd() const
{
    throw std::runtime_error("NOT_IMPLEMENTED: getPollFd");
}

GPIO::Direction PFDigitalGPIO::getDirection() const
{
    assert(_pinNo >= 0 && _pinNo <= 15);
    if (_pinNo < 8)
        return GPIO::Direction::In;
    return GPIO::Direction::Out;
     //throw std::runtime_error("NOT_IMPLEMENTED: getDirection");
}

void PFDigitalGPIO::setDirection(GPIO::Direction direction) const
{
    //throw std::runtime_error("NOT_IMPLEMENTED: setDirection");
}

void PFDigitalGPIO::setValue(GPIO::Value value) const
{
    if (getDirection() == GPIO::Direction::In)
    {
        LOG() << "Ignoring setValue() as this a INPUT pin";
        return;
    }
    pfdigital_provider_->set_pin_value(_pinNo, value == GPIO::Value::High ? true : false);
    //throw std::runtime_error("NOT_IMPLEMENTED setValue");
}

bool PFDigitalGPIO::isActiveLow() const
{
    return false;
  //  throw std::runtime_error("NOT_IMPLEMENTED isActiveLow");
}

void PFDigitalGPIO::setActiveLow(bool state) const
{
    //throw std::runtime_error("NOT_IMPLEMENTED setActiveLow");
}

bool PFDigitalGPIO::hasInterruptsSupport() const
{
    throw std::runtime_error("NOT_IMPLEMENTED hasInterruptsSupport");
}

GPIO::EdgeMode PFDigitalGPIO::getEdgeMode() const
{
     throw std::runtime_error("NOT_IMPLEMENTED getEdgeMode ");
}

void PFDigitalGPIO::setEdgeMode(GPIO::EdgeMode mode) const
{
    throw std::runtime_error("NOT_IMPLEMENTED setEdgeMode");
}
