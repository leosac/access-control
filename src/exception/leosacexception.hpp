/*
    Copyright (C) 2014-2022 Leosac

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
 * \file leosacexception.hpp
 * \brief Exception class for LEOSAC Project related errors
 */

#ifndef LEOSACEXCEPTION_HPP
#define LEOSACEXCEPTION_HPP

#include "tools/Stacktrace.hpp"
#include <cstddef>
#include <exception>
#include <string>

/**
 * A base class for Leosac specific exception.
 *
 * This exception class store a stacktrace of the program at the time it was
 * instantiated (not at the time it was thrown).
 */
class LEOSACException : public std::exception
{
  public:
    explicit LEOSACException(const std::string &message)
        : message_(message)
        , trace_(2) // Skip 2 frames. The LeosacException ctor and Stacktrace ctor.
    {
    }

    virtual ~LEOSACException()
    {
    }

    virtual const char *what() const noexcept final
    {
        return (message_.c_str());
    }

    /**
     * Get the stacktrace associated with this exception.
     */
    const Leosac::Tools::Stacktrace &trace() const
    {
        return trace_;
    }

  protected:
    std::string message_;

  private:
    Leosac::Tools::Stacktrace trace_;
};

#include "configexception.hpp"
#include "coreexception.hpp"
#include "deviceexception.hpp"
#include "dynlibexception.hpp"
#include "fsexception.hpp"
#include "gpioexception.hpp"
#include "moduleexception.hpp"
#include "scriptexception.hpp"
#include "signalexception.hpp"

#endif // LEOSACEXCEPTION_HPP
