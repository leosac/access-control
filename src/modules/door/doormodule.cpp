/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include <sstream>

#include "config/xmlconfig.hpp"
#include "tools/log.hpp"
#include "hardware/device/led.hpp"
#include "hardware/device/buzzer.hpp"
#include "hardware/device/button.hpp"
#include "hardware/device/relay.hpp"
#include "exception/moduleexception.hpp"

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _core(core),
    _hwmanager(core.getHWManager()),
    _name(name),
    _doorRelay(nullptr),
    _doorButton(nullptr),
    _grantedLed(nullptr),
    _deniedLed(nullptr),
    _buzzer(nullptr)
{}

const std::string& DoorModule::getName() const
{
    return (_name);
}

IModule::ModuleType DoorModule::getType() const
{
    return (ModuleType::Door);
}

void DoorModule::serialize(ptree& node)
{
    XmlConfig   conf(_config.doorConf, _doorConfig);

    conf.serialize();

    node.put<std::string>("doorconf", _config.doorConf);
    node.put<std::string>("doorRelay", _config.doorRelay);
    node.put<std::string>("doorButton", _config.doorButton);
    node.put<std::string>("grantedLed", _config.grantedLed);
    node.put<std::string>("deniedLed", _config.deniedLed);
    node.put<std::string>("buzzer", _config.buzzer);
}

void DoorModule::deserialize(const ptree& node)
{

    _config.doorConf = node.get<std::string>("doorconf");
    _config.doorRelay = node.get<std::string>("doorRelay", "none");
    _config.doorButton = node.get<std::string>("doorButton", "none");
    _config.grantedLed = node.get<std::string>("grantedLed", "none");
    _config.deniedLed = node.get<std::string>("deniedLed", "none");
    _config.buzzer = node.get<std::string>("buzzer", "none");
    loadDoorRelay();
    loadDoorButton();
    loadGrantedLed();
    loadDeniedLed();
    loadBuzzer();

    XmlConfig   conf(_config.doorConf, _doorConfig);
    conf.deserialize();
}

bool DoorModule::isAuthRequired() const
{
    return (true); // FIXME
}

void DoorModule::open()
{
    if (_grantedLed)
        _grantedLed->blink(); // DEBUG
    LOG() << "DOOR OPENED !";
    if (_doorRelay)
        _doorRelay->open(); // FIXME
}

bool DoorModule::isOpen() const
{
//     return (_doorSensor->isOpen()); FIXME FIXME FIXME
    return (false);
}

void DoorModule::alarm()
{
    if (_buzzer)
        _buzzer->beep(10000, 500);
    else
        LOG() << "There's no buzzer to buzz.";
    _core.getModuleProtocol().logMessage("Door should be closed !!");
}

void DoorModule::loadDoorRelay()
{
    if (_config.doorRelay == "none")
        return;
    if (!(_doorRelay = dynamic_cast<Relay*>(_hwmanager.getDevice(_config.doorRelay))))
        throw (ModuleException("could not retrieve device \'" + _config.doorRelay + '\''));
}

void DoorModule::loadDoorButton()
{
    if (_config.doorButton == "none")
        return;
    if (!(_doorButton = dynamic_cast<Button*>(_hwmanager.getDevice(_config.doorButton))))
        throw (ModuleException("could not retrieve device \'" + _config.doorButton + '\''));
}

void DoorModule::loadGrantedLed()
{
    if (_config.grantedLed == "none")
        return;
    if (!(_grantedLed = dynamic_cast<Led*>(_hwmanager.getDevice(_config.grantedLed))))
        throw (ModuleException("could not retrieve device \'" + _config.grantedLed + '\''));
}

void DoorModule::loadDeniedLed()
{
    if (_config.deniedLed == "none")
        return;
    if (!(_deniedLed = dynamic_cast<Led*>(_hwmanager.getDevice(_config.deniedLed))))
        throw (ModuleException("could not retrieve device \'" + _config.deniedLed + '\''));
}

void DoorModule::loadBuzzer()
{
    if (_config.buzzer == "none")
        return;
    if (!(_buzzer = dynamic_cast<Buzzer*>(_hwmanager.getDevice(_config.buzzer))))
        throw (ModuleException("could not retrieve device \'" + _config.buzzer + '\''));
}
