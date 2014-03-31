/**
 * \file gpiomanager.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device managerclass
 */

#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "gpio.hpp"

#include <fstream>

class GPIOManager
{
public:
    GPIOManager();
    ~GPIOManager();

private:
    GPIOManager(const GPIOManager& other);
    GPIOManager& operator=(const GPIOManager& other);

public:
    GPIO*   reserve(int id);

private:
    std::ofstream  _exportFile;
    std::ofstream  _unexportFile;
};

#endif // GPIOMANAGER_HPP
