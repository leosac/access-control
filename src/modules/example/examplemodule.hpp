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
    explicit ExampleModule(const std::string& name);
    ~ExampleModule() = default;

    ExampleModule(const ExampleModule& other) = delete;
    ExampleModule& operator=(const ExampleModule& other) = delete;

public:
    virtual void                notify(const Event& event) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

private:
    const std::string   _name;
};

#endif // EXAMPLEMODULE_HPP
