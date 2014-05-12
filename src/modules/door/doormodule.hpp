/**
 * \file doormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#ifndef DOORMODULE_HPP
#define DOORMODULE_HPP

#include "modules/imodule.hpp"

class DoorModule : public IModule
{
public:
    explicit DoorModule();
    ~DoorModule() = default;

    DoorModule(const DoorModule& other) = delete;
    DoorModule& operator=(const DoorModule& other) = delete;

public:
    void                notify(const Event& event);
    ModuleType          getType() const;
    const std::string&  getVersionString() const;

private:
    const std::string   _version;
};

#endif // DOORMODULE_HPP
