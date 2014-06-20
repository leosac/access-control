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
#include "hardware/device/button.hpp"
#include "exception/moduleexception.hpp"

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _core(core),
    _hwmanager(core.getHWManager()),
    _name(name),
    _doorButton(nullptr),
    _grantedLed(nullptr),
    _deniedLed(nullptr)
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
}

void DoorModule::deserialize(const ptree& node)
{

    _config.doorConf = node.get<std::string>("doorconf");
    _config.doorRelay = node.get<std::string>("doorRelay", "none");
    _config.doorButton = node.get<std::string>("doorButton", "none");
    _config.grantedLed = node.get<std::string>("grantedLed", "none");
    _config.deniedLed = node.get<std::string>("deniedLed", "none");
    loadDoorRelay();
    loadDoorButton();
    loadGrantedLed();
    loadDeniedLed();

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
}

void DoorModule::loadDoorRelay()
{
    // TODO
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
