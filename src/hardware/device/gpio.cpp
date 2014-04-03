/**
 * \file gpio.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

#include <cerrno>
#include <iostream> // FIXME Debug

#include "exception/deviceexception.hpp"
#include "tools/unixsyscall.hpp"

const std::string   GPIO::ExportPath        = "/sys/class/gpio/export";
const std::string   GPIO::UnexportPath      = "/sys/class/gpio/unexport";
const std::string   GPIO::GpioPrefix        = "/sys/class/gpio/gpio";
const std::string   GPIO::DirectionFilename = "direction";
const std::string   GPIO::ValueFilename     = "value";
const std::string   GPIO::EdgeFilename      = "edge";
const std::string   GPIO::ActiveLowFilename = "active_low";

static const std::string   edgeStrings[4] = {
    "none",
    "rising",
    "falling",
    "both"
};

GPIO::GPIO(int pinNo)
:   _pinNo(pinNo),
    _path(GpioPrefix + std::to_string(pinNo))
{
    if (!exists())
        exportGpio();

    _directionFile.open(_path + '/' + DirectionFilename);
    _valueFile.open(_path + '/' + ValueFilename);
    _edgeFile.open(_path + '/' + EdgeFilename);
    _activeLowFile.open(_path + '/' + ActiveLowFilename);

    if ((_valueFd = open((_path + '/' + ValueFilename).c_str(), O_RDONLY) == -1))
        throw (DeviceException(UnixSyscall::getErrorString("open", errno)));

    if (!_directionFile.good())
        throw (DeviceException("could not open direction file"));
    else if (!_valueFile.good())
        throw (DeviceException("could not open value file"));
    else if (!_edgeFile.good())
        throw (DeviceException("could not open edge file"));
    else if (!_activeLowFile.good())
        throw (DeviceException("could not open active_low file"));
}

GPIO::~GPIO()
{
    _directionFile.close();
    _valueFile.close();
    _edgeFile.close();
    _activeLowFile.close();

    if (close(_valueFd) == -1)
        throw (DeviceException(UnixSyscall::getErrorString("close", errno)));

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
    if (value == "1")
        return (In);
    else if (value == "0")
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

void GPIO::setEdgeMode(int edgeMode)
{
    const unsigned  bufferLen = 5;
    char            buffer[bufferLen];
    int             ret;

    if (!edgeMode || edgeMode > 3)
        throw (DeviceException("invalid edge mode parameter"));

    _edgeFile.seekp(0);
    _edgeFile << edgeStrings[edgeMode] << std::endl;

    struct pollfd pfd;

    pfd.fd = _valueFd;
    pfd.events = POLLPRI;

    while (1)
    {
        memset(buffer, 0x00, bufferLen);
        lseek(_valueFd, 0, SEEK_SET);
        if ((ret = poll(&pfd, 1, 1000)) == -1)
            throw (DeviceException(UnixSyscall::getErrorString("poll", errno)));
        else if (!ret)
        {
            std::cout << "poll() timeout" << std::endl;
            continue;
        }
        ret = read(_valueFd, buffer, bufferLen - 1);
        if (ret == -1)
            throw (DeviceException(UnixSyscall::getErrorString("read", errno)));
        std::cout << "poll() interrupt caught: val=" << buffer << std::endl;
    }
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

    if (!ex.good())
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
