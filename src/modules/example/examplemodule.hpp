/**
 * \file examplemodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example module class
 */

#ifndef EXAMPLEMODULE_HPP
#define EXAMPLEMODULE_HPP

#include "modules/ieventlistenermodule.hpp"

class ExampleModule : public IEventListenerModule
{
public:
    ExampleModule();
    ~ExampleModule();

private:
    ExampleModule(const ExampleModule& other);
    ExampleModule& operator=(const ExampleModule& other);

public:
    void    sendEvent(const Event& event);

public:
    Type                getType() const;
    const std::string&  getVersionString() const;

private:
    const std::string _version;
};

#endif // EXAMPLEMODULE_HPP
