/**
 * \file dynamiclibrary.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DynamicLibrary class
 */

#include "dynamiclibrary.hpp"
#include "exception/dynlibexception.hpp"

DynamicLibrary::DynamicLibrary(const std::string& file)
:   _file(file),
    _handle(nullptr)
{}

void DynamicLibrary::open(RelocationMode mode)
{
    char*   err;

    if (!(_handle = dlopen(_file.c_str(), static_cast<int>(mode))))
    {
        if ((err = dlerror()))
            throw (DynLibException(std::string("dlopen(): ") + err));
        else
            throw (DynLibException("dlopen(): Unknown error"));
    }
}

void DynamicLibrary::close()
{
    if (dlclose(_handle))
        throw (DynLibException(std::string("dlclose(): ") + dlerror()));
}

void* DynamicLibrary::getSymbol(const std::string& symbol)
{
    void*   sym;
    char*   err;

    sym = dlsym(_handle, symbol.c_str());
    if ((err = dlerror()))
        throw (DynLibException(std::string("dlsym(): ") + err));
    return (sym);
}
