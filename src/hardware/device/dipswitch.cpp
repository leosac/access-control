/**
 * \file dipswitch.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DIPSwitch class implementation
 */

#include "dipswitch.hpp"
#include "exception/deviceexception.hpp"

DIPSwitch::DIPSwitch(const std::string& name, IGPIOProvider& gpioProvider)
:   _name(name),
    _gpioProvider(gpioProvider)
{}

const std::string& DIPSwitch::getName() const
{
    return (_name);
}

void DIPSwitch::serialize(ptree& node)
{
    node.put<std::size_t>("switches", _gpioSwitches.size());
    for (auto& sw : _gpioSwitches)
        sw.serialize(node);
}

void DIPSwitch::deserialize(const ptree& node)
{
    std::size_t switches = node.get<std::size_t>("switches", 0);

    _gpioSwitches.clear();
    for (std::size_t i = 0; i < switches; ++i)
    {
        _gpioSwitches.emplace_back(_gpioProvider, "switch" + std::to_string(i));
        _gpioSwitches.back().deserialize(node);
    }
    _callbacks.resize(_gpioSwitches.size());
}

std::size_t DIPSwitch::getSize() const
{
    return (_gpioSwitches.size());
}

void DIPSwitch::setCallback(std::size_t idx, DIPSwitch::SwitchCallback callback)
{
    if (idx < _callbacks.size())
        _callbacks[idx] = callback;
}

void DIPSwitch::readSwitches()
{
    for (std::size_t i = 0; i < _gpioSwitches.size(); ++i)
    {
        if (_callbacks[i])
            _callbacks[i](_gpioSwitches[i].getGpio()->getValue());
    }
}
