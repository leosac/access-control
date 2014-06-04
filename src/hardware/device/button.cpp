/**
 * \file button.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class implementation
 */

#include "button.hpp"

Button::Button(const std::string& name)
:   _name(name)
{}

const std::string& Button::getName() const
{
    return (_name);
}

void Button::serialize(boost::property_tree::ptree& node)
{
    static_cast<void>(node); // FIXME
}

void Button::deserialize(const boost::property_tree::ptree& node)
{
    static_cast<void>(node); // FIXME
}
