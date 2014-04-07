/**
 * \file examplemoduleloader.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example moduleloader class
 */

#ifndef EXAMPLEMODULELOADER_HPP
#define EXAMPLEMODULELOADER_HPP

#include "modules/amoduleloader.hpp"

class ExampleModuleLoader : public AModuleLoader
{
public:
    ExampleModuleLoader();
    ~ExampleModuleLoader();

public:
    IModule*    instanciateModule() const;
};

#endif // EXAMPLEMODULELOADER_HPP
