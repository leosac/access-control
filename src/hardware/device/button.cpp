/**
 * \file button.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class implementation
 */

#include "button.hpp"

IDevice::DeviceType Button::getType() const
{
    return (DeviceType::Button);
}

void Button::serialize(boost::property_tree::ptree& node)
{
    static_cast<void>(node); // FIXME
}

void Button::deserialize(const boost::property_tree::ptree& node)
{
    static_cast<void>(node); // FIXME
}
