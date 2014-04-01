/**
 * \file examplemodule.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief example module class
 */

#ifndef EXAMPLEMODULE_HPP
#define EXAMPLEMODULE_HPP

#include "modules/imodule.hpp"

class ExampleModule : public IModule
{
public:
    ExampleModule();
    ~ExampleModule();

private:
    ExampleModule(const ExampleModule& other);
    ExampleModule& operator=(const ExampleModule& other);

public:
    void    sayHello() const;
};

#endif // EXAMPLEMODULE_HPP
