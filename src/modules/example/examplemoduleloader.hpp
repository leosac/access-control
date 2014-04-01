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
