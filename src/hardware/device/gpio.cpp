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
#include <stdlib.h>

#include <cerrno>
#include <iostream> // FIXME Debug

#include "exception/deviceexception.hpp"
#include "tools/unixsyscall.hpp"

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
    _path(GpioPrefix + std::to_string(pinNo))
{
    if (!exists())
        exportGpio();
    _directionFile.open(_path + '/' + DirectionFilename);
//     _valueFile.open(_path + '/' + ValueFilename);
    _activeLowFile.open(_path + '/' + ActiveLowFilename);
    _edgeFile.open(_path + '/' + EdgeFilename);
    if (!_directionFile.good())
        throw (DeviceException("could not open direction file"));
//     else if (!_valueFile.good())
//         throw (DeviceException("could not open value file"));
    else if (!_activeLowFile.good())
        throw (DeviceException("could not open active_low file"));
    else if (!_edgeFile.good())
        throw (DeviceException("could not open edge file"));
}

GPIO::~GPIO()
{
    _directionFile.close();
//     _valueFile.close();
    _activeLowFile.close();
    _edgeFile.close();
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
    std::string ret;

    _directionFile.seekp(0);
    _directionFile >> ret;
    if (ret == "in")
        return (In);
    else if (ret == "out")
        return (Out);
    else
        throw (DeviceException("direction read error (read " + ret + ")"));
}

void GPIO::setDirection(Direction direction)
{
    _directionFile.seekp(0);
    if (direction == In)
        _directionFile << "in" << std::endl;
    else if (direction == Out)
        _directionFile << "out" << std::endl;
    else
        throw (DeviceException("invalid direction parameter"));
}

bool GPIO::getValue()
{
    std::string ret;

    _valueFile.seekp(0);
    _valueFile >> ret;
    if (ret == "1")
        return (true);
    else if (ret == "0")
        return (false);
    else
        throw (DeviceException("invalid value"));
}

void GPIO::setValue(bool state)
{
    _valueFile.seekp(0);
    _valueFile << ((state) ? ("1") : ("0")) << std::endl;
}

bool GPIO::isActiveLow()
{
    std::string ret;

    _activeLowFile.seekp(0);
    _activeLowFile >> ret;
    if (ret == "0")
        return (false);
    else if (ret == "1")
        return (true);
    else
        throw (DeviceException("invalid active state"));
}

void GPIO::setActiveLow(bool state)
{
    _activeLowFile.seekp(0);
    _activeLowFile << ((state) ? ("1") : ("0")) << std::endl;
}

GPIO::EdgeMode GPIO::getEdgeMode()
{
    std::string ret;

    _edgeFile.seekp(0);
    _edgeFile >> ret;
    for (int i = 0; i < EdgeModes; ++i)
    {
        if (ret == EdgeStrings[i])
            return (static_cast<EdgeMode>(i));
    }
    throw (DeviceException("invalid edge mode"));
}

void GPIO::setEdgeMode(EdgeMode mode)
{
    const unsigned  bufferLen = 64;
    char            buffer[bufferLen];
    int             ret;

    if (mode < 0 || mode >= EdgeModes)
        throw (DeviceException("invalid edge mode parameter"));

    _edgeFile.seekp(0);
    _edgeFile << EdgeStrings[mode] << std::endl;

    if (mode == None)
        return;

    std::cout << "EdgeMode=" << getEdgeMode() << std::endl;

    struct pollfd   fdset[2];
    int             gpioFd; // NOTE Used for polling

    std::cout << "File=" << (_path + '/' + ValueFilename).c_str() << std::endl;
    if ((gpioFd = open((_path + '/' + ValueFilename).c_str(), O_RDONLY | O_NONBLOCK) == -1))
        throw (DeviceException(UnixSyscall::getErrorString("open", errno)));

    while (42)
    {
        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = STDIN_FILENO;
        fdset[0].events = POLLIN;

        fdset[1].fd = gpioFd;
        fdset[1].events = POLLPRI;

        if ((ret = poll(fdset, 2, 3000)) == -1)
            throw (DeviceException(UnixSyscall::getErrorString("poll", errno)));
        if (ret == 0)
        {
            std::cout << "timeout" << std::endl;
        }

        if (fdset[1].revents & POLLPRI)
        {
            if ((ret = read(fdset[1].fd, buffer, bufferLen - 1)) == -1)
                throw (DeviceException(UnixSyscall::getErrorString("read", errno)));
            if (ret > 0)
                buffer[ret] = '\0';
            if (lseek(gpioFd, 0, SEEK_SET) == -1)
                throw (DeviceException(UnixSyscall::getErrorString("lseek", errno)));
            std::cout << "poll() interrupt caught: buffer=" << buffer << std::endl;
        }

        if (fdset[0].revents & POLLIN)
        {
            if ((ret = read(fdset[0].fd, buffer, bufferLen - 1)) == -1)
                throw (DeviceException(UnixSyscall::getErrorString("read", errno)));
            if (ret > 0)
                buffer[ret] = '\0';
            std::cout << "poll() user input caught: buffer=" << buffer << std::endl;
        }
    }

    if (close(gpioFd) == -1)
        throw (DeviceException(UnixSyscall::getErrorString("close", errno)));
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
