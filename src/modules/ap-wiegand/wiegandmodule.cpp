/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include <sstream>

WiegandModule::WiegandModule(ICore& core, const std::string& name)
:   _listener(core),
    _name(name),
    _hiGPIO(0),
    _loGPIO(0),
    _hwmanager(core.getHWManager()),
    _interface(nullptr)
{}

void WiegandModule::notifyCardRead(const IWiegandListener::CardId& cardId)
{
    std::ostringstream  oss;

    for (std::size_t i = 0; i < cardId.size(); ++i)
    {
        if (i > 0)
            oss << ' ';
        oss << static_cast<unsigned int>(cardId[i]);
    }
    _listener.notify(Event(oss.str(), _name, _target));
}

void WiegandModule::notify(const Event& event)
{
    static_cast<void>(event);
}

const std::string& WiegandModule::getName() const
{
    return (_name);
}

IModule::ModuleType WiegandModule::getType() const
{
    return (ModuleType::AccessPoint);
}

void WiegandModule::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree& child = node.add("properties", std::string());

    child.put("target", _target);
    child.put("higpio", _hiGPIO);
    child.put("logpio", _loGPIO);
    delete _interface;
}

void WiegandModule::deserialize(const boost::property_tree::ptree& node)
{
    for (auto& v : node)
    {
        if (v.first == "properties")
        {
            _target = v.second.get<std::string>("target");
            _hiGPIO = v.second.get<unsigned int>("higpio");
            _loGPIO = v.second.get<unsigned int>("logpio");
            _interface = _hwmanager.buildWiegandInterface(this, _hiGPIO, _loGPIO);
        }
    }
}
