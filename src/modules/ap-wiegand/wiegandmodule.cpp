/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include "exception/moduleexception.hpp"
#include "core/moduleprotocol/moduleprotocol.hpp"
#include "core/moduleprotocol/authcommands/authcmdcreaterequest.hpp"
#include "hardware/device/wiegandreader.hpp"

WiegandModule::WiegandModule(ICore& core, const std::string& name)
:   _core(core),
    _name(name),
    _hiGPIO(0),
    _loGPIO(0),
    _hwmanager(core.getHWManager())
{}

const std::string& WiegandModule::getName() const
{
    return (_name);
}

IModule::ModuleType WiegandModule::getType() const
{
    return (ModuleType::AccessPoint);
}

void WiegandModule::serialize(ptree& node)
{
    for (auto requester : _requesterList)
        delete requester;
    _requesterList.clear();
    node.put("target", _target);
    node.put("readerDevice", _deviceName);
}

void WiegandModule::deserialize(const ptree& node)
{
    WiegandReader*  device;

    _deviceName = node.get<std::string>("readerDevice");
    _target = node.get<std::string>("target");
    if (!(device = dynamic_cast<WiegandReader*>(_hwmanager.getDevice(_deviceName))))
        throw (ModuleException("could not get reader device"));
    _requesterList.push_back(new WiegandRequester(*this, device));
}

void WiegandModule::notifyAccess(const std::string& request)
{
    std::lock_guard<std::mutex> lg(_notifyMutex);

    _core.getModuleProtocol().pushCommand(ICommand::Ptr(new AuthCmdCreateRequest(&_core.getModuleProtocol(), _name, _target, request)));
    _core.getModuleProtocol().notifyMonitor(IModuleProtocol::ActivityType::Auth);
}
