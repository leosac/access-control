/**
 * \file gpiomanager.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device manager class
 */

#include "gpiomanager.hpp"

GPIOManager::GPIOManager() {}

GPIOManager::~GPIOManager() {}

GPIOManager::GPIOManager(const GPIOManager& /*other*/) {}

GPIOManager& GPIOManager::operator=(const GPIOManager& /*other*/)
{
    return (*this);
}

GPIO* GPIOManager::reserve(int id)
{
    return (new GPIO(id));
}
