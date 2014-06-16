/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include <sstream>

#include "hardware/device/wiegandreader.hpp"
#include "exception/moduleexception.hpp"
#include "core/moduleprotocol/moduleprotocol.hpp"
#include <core/moduleprotocol/authcommands/authcmdcreaterequest.hpp>

WiegandModule::WiegandModule(ICore& core, const std::string& name)
:   _core(core),
    _name(name),
    _hiGPIO(0),
    _loGPIO(0),
    _hwmanager(core.getHWManager()),
    _interface(nullptr)
{}

void WiegandModule::notifyCardRead(const IWiegandListener::CardId& cardId)
{
    std::ostringstream  oss; // FIXME encode in xml

    for (std::size_t i = 0; i < cardId.size(); ++i)
    {
        if (i > 0)
            oss << ' ';
        oss << static_cast<unsigned int>(cardId[i]);
    }
    _core.getModuleProtocol().pushAuthCommand(new AuthCmdCreateRequest(&_core.getModuleProtocol(), _name, _target, oss.str()));
    _core.getModuleProtocol().notifyMonitor(IModuleProtocol::ActivityType::Auth); // DEBUG
}

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
    _interface->unregisterListener(this);
    node.put("target", _target);
    node.put("readerDevice", _interfaceName);
}

void WiegandModule::deserialize(const ptree& node)
{
    _interfaceName = node.get<std::string>("readerDevice");
    _target = node.get<std::string>("target");
    if (!(_interface = dynamic_cast<WiegandReader*>(_hwmanager.getDevice(_interfaceName))))
        throw (ModuleException("could not get reader device"));
    _interface->registerListener(this);
}
