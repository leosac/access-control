/**
 * \file doormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#ifndef DOORMODULE_HPP
#define DOORMODULE_HPP

#include "modules/idoormodule.hpp"

class Led;

class DoorModule : public IDoorModule
{
    typedef struct {
        bool    open;
        int     start;
        int     end;
    } Day;

public:
    explicit DoorModule(ICore& core, const std::string& name);
    ~DoorModule() = default;

    DoorModule(const DoorModule& other) = delete;
    DoorModule& operator=(const DoorModule& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;
    virtual bool                isAuthRequired() const override;
    virtual void                open() override;


private:
    bool    isDoorOpenable();

private:
    ICore&              _core;
    IHWManager&         _hwmanager;
    const std::string   _name;
    Led*                _grantedLed;
    std::string         _grantedLedName;
    Day                 _days[7];
};

#endif // DOORMODULE_HPP
