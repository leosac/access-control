/**
 * \file gpio.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

extern "C" {
#include <fcntl.h>
#include <unistd.h>
}

#include <fstream>

#include "exception/gpioexception.hpp"
#include "tools/unixsyscall.hpp"
#include "tools/log.hpp"

const std::string   GPIO::ExportPath        = "/sys/class/gpio/export";
const std::string   GPIO::UnexportPath      = "/sys/class/gpio/unexport";
const std::string   GPIO::GpioPathPrefix    = "/sys/class/gpio/";
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

GPIO::GPIO(int pinNo/*, const std::string& sysfsName*/)
:   _pinNo(pinNo),
    _path(GpioPathPrefix + "gpio" + std::to_string(_pinNo) + "_ph" + std::to_string(_pinNo + 19)), // NOTE cubieboard2 fix FIXME asap
    _directionFile(_path + '/' + DirectionFilename),
    _valueFile(_path + '/' + ValueFilename),
    _activeLowFile(_path + '/' + ActiveLowFilename),
    _edgeFile(_path + '/' + EdgeFilename),
    _pollFd(-1)
{
    LOG() << "GPIO number " << _pinNo;
    if (!exists())
        exportGpio();
    LOG() << "Opening file " << _valueFile;
    if ((_pollFd = ::open(_valueFile.c_str(), O_RDONLY | O_NONBLOCK)) == -1)
        throw (GpioException(UnixSyscall::getErrorString("open", errno)));
}

GPIO::~GPIO()
{
    try
    {
        if (::close(_pollFd) == -1)
            throw (GpioException(UnixSyscall::getErrorString("close", errno)));
        if (exists())
            unexportGpio();
    }
    catch (const GpioException& e) {}
}

int GPIO::getPinNo() const
{
    return (_pinNo);
}

const std::string& GPIO::getPath() const
{
    return (_path);
}

int GPIO::getPollFd() const
{
    return (_pollFd);
}

GPIO::Direction GPIO::getDirection()
{
    std::string     ret;
    std::fstream    file(_directionFile, std::ios::in);

    if (!file.good())
        throw (GpioException("could not open " + _directionFile));
    file.seekp(0);
    file >> ret;
    if (ret == "in")
        return (In);
    else if (ret == "out")
        return (Out);
    else
        throw (GpioException("direction read error (read " + ret + ")"));
}

void GPIO::setDirection(Direction direction)
{
    std::fstream    file(_directionFile, std::ios::out);

    if (!file.good())
        throw (GpioException("could not open " + _directionFile));
    file.seekp(0);
    if (direction == In)
        file << "in";
    else if (direction == Out)
        file << "out";
    else
        throw (GpioException("invalid direction parameter"));
}

bool GPIO::getValue()
{
    std::string     ret;
    std::fstream    file(_valueFile, std::ios::in);

    if (!file.good())
        throw (GpioException("could not open " + _valueFile));
    file.seekp(0);
    file >> ret;
    if (ret == "1")
        return (true);
    else if (ret == "0")
        return (false);
    else
        throw (GpioException("invalid value"));
}

void GPIO::setValue(bool state)
{
    std::fstream    file(_valueFile, std::ios::out);

    if (!file.good())
        throw (GpioException("could not open " + _valueFile));
    file.seekp(0);
    file << ((state) ? ("1") : ("0"));
}

bool GPIO::isActiveLow()
{
    std::string     ret;
    std::fstream    file(_activeLowFile, std::ios::in);

    if (!file.good())
        throw (GpioException("could not open " + _activeLowFile));
    file.seekp(0);
    file >> ret;
    if (ret == "0")
        return (false);
    else if (ret == "1")
        return (true);
    else
        throw (GpioException("invalid active state"));
}

void GPIO::setActiveLow(bool state)
{
    std::fstream    file(_activeLowFile, std::ios::out);

    if (!file.good())
        throw (GpioException("could not open " + _activeLowFile));
    file.seekp(0);
    file << ((state) ? ("1") : ("0"));
}

GPIO::EdgeMode GPIO::getEdgeMode()
{
    std::string     ret;
    std::fstream    file(_edgeFile, std::ios::in);

    if (!file.good())
        throw (GpioException("could not open " + _edgeFile));
    file.seekp(0);
    file >> ret;
    for (int i = 0; i < EdgeModes; ++i)
    {
        if (ret == EdgeStrings[i])
            return (static_cast<EdgeMode>(i));
    }
    throw (GpioException("invalid edge mode"));
}

void GPIO::setEdgeMode(EdgeMode mode)
{
    std::fstream    file(_edgeFile, std::ios::out);

    if (!file.good())
        throw (GpioException("could not open " + _edgeFile));

    if (mode < 0 || mode >= EdgeModes)
        throw (GpioException("invalid edge mode parameter"));

    file.seekp(0);
    file << EdgeStrings[mode];
}

bool GPIO::exists()
{
    std::fstream    gpio(_path);
    bool            rslt;

    rslt = gpio.good();
    LOG() << "rslt=" << rslt;
    return (rslt);
}

void GPIO::exportGpio()
{
    std::fstream    ex(ExportPath, std::ios::out);

    LOG() << "Opening file " << ExportPath;
    if (!ex.good())
        throw (GpioException("could not export gpio " + std::to_string(_pinNo)));
    ex << std::to_string(_pinNo);
}

void GPIO::unexportGpio()
{
    std::fstream    unex(UnexportPath, std::ios::out);

    LOG() << "Opening file " << UnexportPath;
    if (!unex.good())
        throw (GpioException("could not unexport gpio " + std::to_string(_pinNo)));
    unex << std::to_string(_pinNo);
}
