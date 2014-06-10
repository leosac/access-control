/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include <sstream>

#include "hardware/device/wiegandreader.hpp"
#include "exception/moduleexception.hpp"

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
    _core.sendAuthRequest(oss.str());
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
    ptree& properties = node.add("properties", std::string());

    properties.put("target", _target);
    properties.put("readerDevice", _interfaceName);
}

void WiegandModule::deserialize(const ptree& node)
{
    ptree properties = node.get_child("properties");

    _interfaceName = properties.get<std::string>("readerDevice");
    if (!(_interface = dynamic_cast<WiegandReader*>(_hwmanager.getDevice(_interfaceName))))
        throw (ModuleException("could not get reader device"));
}
