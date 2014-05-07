/**
 * \file examplemodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example module class
 */

#ifndef EXAMPLEMODULE_HPP
#define EXAMPLEMODULE_HPP

#include "modules/imodule.hpp"

class ExampleModule : public IModule
{
public:
    explicit ExampleModule();
    ~ExampleModule() = default;

    ExampleModule(const ExampleModule& other) = delete;
    ExampleModule& operator=(const ExampleModule& other) = delete;

public:
    void                notify(const Event& event);
    Type                getType() const;
    const std::string&  getVersionString() const;

private:
    const std::string   _version;
};

#endif // EXAMPLEMODULE_HPP
