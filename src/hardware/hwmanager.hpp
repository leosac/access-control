/**
 * \file hwmanager.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief hardware managing class
 */

#ifndef HWMANAGER_HPP
#define HWMANAGER_HPP

#include "ihwmanager.hpp"

class HWManager : public IHWManager
{
public:
    HWManager();
    ~HWManager();
};

#endif // HWMANAGER_HPP
