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

#include "exception/gpioexception.hpp"
#include "tools/unixsyscall.hpp"
#include "tools/log.hpp"
#include "tools/unixfs.hpp"

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
    std::string     ret = UnixFs::readSysFsValue<std::string>(_directionFile);

    if (ret == "in")
        return (Direction::In);
    else if (ret == "out")
        return (Direction::Out);
    else
        throw (GpioException("direction read error (read " + ret + ')'));
}

void GPIO::setDirection(Direction direction)
{
    if (direction == Direction::In)
        UnixFs::writeSysFsValue<std::string>(_directionFile, "in");
    else if (direction == Direction::Out)
        UnixFs::writeSysFsValue<std::string>(_directionFile, "out");
    else
        throw (GpioException("invalid direction parameter"));
}

bool GPIO::getValue()
{
    std::string     ret = UnixFs::readSysFsValue<std::string>(_valueFile);

    if (ret == "1")
        return (true);
    else if (ret == "0")
        return (false);
    else
        throw (GpioException("invalid value"));
}

void GPIO::setValue(bool state)
{
    UnixFs::writeSysFsValue<std::string>(_valueFile, ((state) ? ("1") : ("0")));
}

bool GPIO::isActiveLow()
{
    std::string     ret = UnixFs::readSysFsValue<std::string>(_activeLowFile);

    if (ret == "0")
        return (false);
    else if (ret == "1")
        return (true);
    else
        throw (GpioException("invalid active state"));
}

void GPIO::setActiveLow(bool state)
{
    UnixFs::writeSysFsValue<std::string>(_activeLowFile, ((state) ? ("1") : ("0")));
}

GPIO::EdgeMode GPIO::getEdgeMode()
{
    std::string     ret = UnixFs::readSysFsValue<std::string>(_edgeFile);

    for (int i = 0; i < EdgeModes; ++i)
    {
        if (ret == EdgeStrings[i])
            return (static_cast<EdgeMode>(i));
    }
    throw (GpioException("invalid edge mode"));
}

void GPIO::setEdgeMode(EdgeMode mode)
{
    UnixFs::writeSysFsValue<std::string>(_edgeFile, EdgeStrings[static_cast<int>(mode)]);
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
    UnixFs::writeSysFsValue<int>(ExportPath, _pinNo);
}

void GPIO::unexportGpio()
{
    UnixFs::writeSysFsValue<int>(UnexportPath, _pinNo);
}
