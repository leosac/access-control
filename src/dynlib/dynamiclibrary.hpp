/**
 * \file dynamiclibrary.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DynamicLibrary class
 */

#ifndef DYNAMICLIBRARY_HPP
#define DYNAMICLIBRARY_HPP

extern "C" {
#include <dlfcn.h>
}

#include <string>

/**
* Wraps a dynamic library handler and provide methods to interact with it.
*/
class DynamicLibrary
{
public:
    enum class RelocationMode : int {
        Lazy    = RTLD_LAZY,
        Now     = RTLD_NOW
    };

public:
    /**
    * Construct a dynamic library wrapper for the shared object referenced by name.
    * @param file is the path to the dynamic library file.
    */
    explicit DynamicLibrary(const std::string& file);
    ~DynamicLibrary() = default;

    DynamicLibrary(const DynamicLibrary& other) = delete;
    DynamicLibrary& operator=(const DynamicLibrary& other) = delete;

public:
    /**
    * Attempts to open the shared library file so that we can access its symbols.
    * You must call open() before calling getSymbol().
    * @throws DynLibException if something went wrong.
    */
    void    open(RelocationMode mode = RelocationMode::Lazy);

    /**
    * Close the already opened library handler.
    * @throws DynLibException if something went wrong.
    */
    void    close();

    /**
    * Lookup a symbol by name and return a pointer to it.
    * You need to open() the library first.
    * @throws DynLibException if we cannot access the symbol.
    */
    void*   getSymbol(const std::string& symbol);

    /**
    * Returns the full path from which the library was loaded.
    */
    const std::string &getFilePath() const;

private:
    std::string _file;
    void*       _handle;
};

#endif // DYNAMICLIBRARY_HPP
