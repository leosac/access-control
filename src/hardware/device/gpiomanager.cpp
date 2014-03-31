/**
 * \file gpiomanager.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device manager class
 */

#include "gpiomanager.hpp"

#include <sstream>

#include "exception/deviceexception.hpp"

static const char* exportPath = "/sys/class/gpio/export";
static const char* unexportPath = "/sys/class/gpio/unexport";

GPIOManager::GPIOManager()
:   _exportFile(exportPath),
    _unexportFile(unexportPath)
{
    if (!_exportFile.good())
        throw (DeviceException(std::string("could not open file ") + exportPath));
    if (!_unexportFile.good())
        throw (DeviceException(std::string("could not open file ") + unexportPath));
}

GPIOManager::~GPIOManager() {}

GPIOManager::GPIOManager(const GPIOManager& /*other*/) {}

GPIOManager& GPIOManager::operator=(const GPIOManager& /*other*/)
{
    return (*this);
}

GPIO* GPIOManager::reserve(int id)
{
    std::stringstream   ss;
    std::string         idString;

    ss << id << std::endl;
    idString = ss.str();
    _exportFile.write(idString.c_str(), idString.size());
    
//     _unexportFile.write(idString.c_str(), idString.size());
    return (new GPIO(id));
}
