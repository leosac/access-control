/*
    Copyright (C) 2014-2016 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file deviceexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Device related errors
 */

#ifndef DEVICEEXCEPTION_HPP
#define DEVICEEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class DeviceException : public LEOSACException
{
  public:
    DeviceException(const std::string &message)
        : LEOSACException("Device::" + message)
    {
    }
    virtual ~DeviceException()
    {
    }
};

#endif // DEVICEEXCEPTION_HPP
