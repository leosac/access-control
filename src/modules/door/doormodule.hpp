/**
 * \file doormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#ifndef DOORMODULE_HPP
#define DOORMODULE_HPP

#include "modules/imodule.hpp"

class Led;

class DoorModule : public IModule
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
    virtual void                notify(const Event& event) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(boost::property_tree::ptree& node) override;
    virtual void                deserialize(const boost::property_tree::ptree& node) override;

private:
    void    open();
    bool    isDoorOpenable();

private:
    IEventListener&     _listener;
    IHWManager&         _hwmanager;
    const std::string   _name;
    Led*                _grantedLed;
    std::string         _grantedLedName;
    Day                 _days[7];
};

#endif // DOORMODULE_HPP
