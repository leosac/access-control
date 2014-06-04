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

void Button::serialize(ptree& node)
{
    static_cast<void>(node); // FIXME
}

void Button::deserialize(const ptree& node)
{
    static_cast<void>(node); // FIXME
}
