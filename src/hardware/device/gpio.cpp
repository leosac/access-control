/**
 * \file gpio.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

#include <fstream>

#include "exception/deviceexception.hpp"

const std::string   GPIO::ExportPath        = "/sys/class/gpio/export";
const std::string   GPIO::UnexportPath      = "/sys/class/gpio/unexport";
const std::string   GPIO::GpioPrefix        = "/sys/class/gpio/gpio";
const std::string   GPIO::DirectionFilename = "direction";
const std::string   GPIO::ValueFilename     = "value";
const std::string   GPIO::ActiveLowFilename = "active_low";
const std::string   GPIO::EdgeFilename      = "edge";
const std::string   GPIO::EdgeStrings[EdgeModes] = {
    "none",
    "rising",
    "falling",
    "both"
};

GPIO::GPIO(int pinNo)
:   _pinNo(pinNo),
    _path(GpioPrefix + std::to_string(_pinNo)),
    _directionFile(_path + '/' + DirectionFilename),
    _valueFile(_path + '/' + ValueFilename),
    _activeLowFile(_path + '/' + ActiveLowFilename),
    _edgeFile(_path + '/' + EdgeFilename),
    _poller(_valueFile)
{
    if (!exists())
        exportGpio();
}

GPIO::~GPIO()
{
    if (_poller.isRunning())
        _poller.stop();
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

GPIO::Direction GPIO::getDirection()
{
    std::string     ret;
    std::fstream    file(_directionFile, std::ios::in);

    if (!file.good())
        throw (DeviceException("could not open " + _directionFile));
    file.seekp(0);
    file >> ret;
    if (ret == "in")
        return (In);
    else if (ret == "out")
        return (Out);
    else
        throw (DeviceException("direction read error (read " + ret + ")"));
}

void GPIO::setDirection(Direction direction)
{
    std::fstream    file(_directionFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _directionFile));
    file.seekp(0);
    if (direction == In)
        file << "in";
    else if (direction == Out)
        file << "out";
    else
        throw (DeviceException("invalid direction parameter"));
}

bool GPIO::getValue()
{
    std::string     ret;
    std::fstream    file(_valueFile, std::ios::in);

    if (!file.good())
        throw (DeviceException("could not open " + _valueFile));
    file.seekp(0);
    file >> ret;
    if (ret == "1")
        return (true);
    else if (ret == "0")
        return (false);
    else
        throw (DeviceException("invalid value"));
}

void GPIO::setValue(bool state)
{
    std::fstream    file(_valueFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _valueFile));
    file.seekp(0);
    file << ((state) ? ("1") : ("0"));
}

bool GPIO::isActiveLow()
{
    std::string     ret;
    std::fstream    file(_activeLowFile, std::ios::in);

    if (!file.good())
        throw (DeviceException("could not open " + _activeLowFile));
    file.seekp(0);
    file >> ret;
    if (ret == "0")
        return (false);
    else if (ret == "1")
        return (true);
    else
        throw (DeviceException("invalid active state"));
}

void GPIO::setActiveLow(bool state)
{
    std::fstream    file(_activeLowFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _activeLowFile));
    file.seekp(0);
    file << ((state) ? ("1") : ("0"));
}

GPIO::EdgeMode GPIO::getEdgeMode()
{
    std::string     ret;
    std::fstream    file(_edgeFile, std::ios::in);

    if (!file.good())
        throw (DeviceException("could not open " + _edgeFile));
    file.seekp(0);
    file >> ret;
    for (int i = 0; i < EdgeModes; ++i)
    {
        if (ret == EdgeStrings[i])
            return (static_cast<EdgeMode>(i));
    }
    throw (DeviceException("invalid edge mode"));
}

void GPIO::setEdgeMode(EdgeMode mode)
{
    std::fstream    file(_edgeFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _edgeFile));

    if (mode < 0 || mode >= EdgeModes)
        throw (DeviceException("invalid edge mode parameter"));

    file.seekp(0);
    file << EdgeStrings[mode];
}

void GPIO::startPolling()
{
    _poller.start();
}

void GPIO::stopPolling()
{
    _poller.stop();
}

bool GPIO::exists()
{
    std::fstream    gpio(_path);
    bool            rslt;

    rslt = gpio.good();
    return (rslt);
}

void GPIO::exportGpio()
{
    std::fstream    ex(ExportPath, std::ios::out);

    if (!ex.good())
        throw (DeviceException("could not export gpio"));
    ex << std::to_string(_pinNo);
}

void GPIO::unexportGpio()
{
    std::fstream    unex(ExportPath, std::ios::out);

    if (!unex.good())
        throw (DeviceException("could not unexport gpio"));
    unex << std::to_string(_pinNo);
}
