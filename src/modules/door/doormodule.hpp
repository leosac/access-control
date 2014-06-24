/**
 * \file doormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#ifndef DOORMODULE_HPP
#define DOORMODULE_HPP

#include "modules/idoormodule.hpp"
#include "doorconfig.hpp"

class Relay;
class Buzzer;
class Button;
class Led;

class DoorModule : public IDoorModule
{
    typedef struct {
        std::string     doorRelay;
        std::string     doorButton;
        std::string     grantedLed;
        std::string     deniedLed;
        std::string     doorConf;
        std::string     buzzer;
    } Config;
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
    virtual bool                isOpen() const override;
    virtual void                alarm() override;

private:
    void    loadDoorRelay();
    void    loadDoorButton();
    void    loadGrantedLed();
    void    loadDeniedLed();
    void    loadBuzzer();

private:
    ICore&              _core;
    IHWManager&         _hwmanager;
    const std::string   _name;
    std::string         _configFile;
    DoorConfig          _doorConfig;
    Relay*              _doorRelay;
    Button*             _doorButton;
    Led*                _grantedLed;
    Led*                _deniedLed;
    Buzzer*             _buzzer;
    Config              _config;
};

#endif // DOORMODULE_HPP
