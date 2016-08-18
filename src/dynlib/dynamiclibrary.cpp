/*
    Copyright (C) 2014-2015 Islog

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
 * \file dynamiclibrary.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DynamicLibrary class
 */

#include "dynamiclibrary.hpp"
#include "exception/dynlibexception.hpp"

DynamicLibrary::DynamicLibrary(const std::string &file)
    : _file(file)
    , _handle(nullptr)
{
}

void DynamicLibrary::open(RelocationMode mode)
{
    char *err;

    if (!(_handle = dlopen(_file.c_str(), static_cast<int>(mode) | RTLD_NODELETE)))
    {
        if ((err = dlerror()))
            throw(DynLibException(std::string("dlopen(): ") + err));
        else
            throw(DynLibException("dlopen(): Unknown error"));
    }
}

void DynamicLibrary::close()
{
    if (dlclose(_handle))
        throw(DynLibException(std::string("dlclose(): ") + dlerror()));
}

void *DynamicLibrary::getSymbol(const std::string &symbol)
{
    void *sym;
    char *err;

    sym = dlsym(_handle, symbol.c_str());
    if ((err = dlerror()))
        throw(DynLibException(std::string("dlsym(): ") + err));
    return (sym);
}

const std::string &DynamicLibrary::getFilePath() const
{
    return _file;
}
