/**
 * \file gpio.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

#include "exception/deviceexception.hpp"

const std::string   GPIO::ExportPath        = "/sys/class/gpio/export";
const std::string   GPIO::UnexportPath      = "/sys/class/gpio/unexport";
const std::string   GPIO::GpioPrefix        = "/sys/class/gpio/gpio";
const std::string   GPIO::DirectionFilename = "direction";
const std::string   GPIO::ValueFilename     = "value";

GPIO::GPIO(int pinNo)
:   _pinNo(pinNo),
    _path(GpioPrefix + std::to_string(pinNo))
{
    if (!exists())
        exportGpio();
    _directionFile.open(_path + '/' + DirectionFilename);
    _valueFile.open(_path + '/' + ValueFilename);
    if (!_directionFile.is_open())
        throw (DeviceException("could not open direction file"));
    if (!_valueFile.is_open())
        throw (DeviceException("could not open value file"));
}

GPIO::~GPIO()
{
    _directionFile.close();
    _valueFile.close();
    if (exists())
        unexportGpio();
}

int GPIO::getPinNo() const
{
    return (_pinNo);
}

const std::string& GPIO::getPath() const
{
    return (_path);
}

int GPIO::getDirection()
{
    std::string value;

    _valueFile.seekp(0);
    _valueFile >> value;
    if (value == std::to_string(In))
        return (In);
    else if (value == std::to_string(Out))
        return (Out);
    else
        throw (DeviceException("direction read error (read " + value + ")"));
}

void GPIO::setDirection(int direction)
{
    _directionFile.seekp(0);
    if (direction == In)
        _directionFile << "in" << std::endl;
    else if (direction == Out)
        _directionFile << "out" << std::endl;
    else
        throw (DeviceException("invalid direction parameter"));
}

int GPIO::getValue()
{
    std::string ret;
    int         value;

    _valueFile.seekp(0);
    _valueFile >> ret;
    value = (ret != std::to_string(Low));
    return (value);
}

void GPIO::setValue(int value)
{
    _valueFile.seekp(0);
    if (value == Low)
        _valueFile << std::to_string(Low) << std::endl;
    else if (value == High)
        _valueFile << std::to_string(High) << std::endl;
    else
        throw (DeviceException("invalid value parameter"));
}

bool GPIO::exists()
{
    std::fstream    gpio(_path);
    bool            rslt;

    rslt = gpio.good();
    gpio.close();
    return (rslt);
}

void GPIO::exportGpio()
{
    std::fstream    ex(ExportPath, std::ios::out);

    if (!ex.is_open())
        throw (DeviceException("could not export gpio"));
    ex << std::to_string(_pinNo);
    ex.close();
}

void GPIO::unexportGpio()
{
    std::fstream    unex(ExportPath, std::ios::out);

    if (!unex.is_open())
        throw (DeviceException("could not unexport gpio"));
    unex << std::to_string(_pinNo);
    unex.close();
}
