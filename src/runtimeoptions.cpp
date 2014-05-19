/**
 * \file runtimeoptions.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief RuntimeOptions class implementation
 */

#include "runtimeoptions.hpp"

const std::string RuntimeOptions::DefaultEmptyParam("undef");

void RuntimeOptions::setFlag(Flag flag, bool value)
{
    _flags.set(flag, value);
}

bool RuntimeOptions::isSet(Flag flag) const
{
    return (_flags.test(flag));
}

void RuntimeOptions::setParam(const std::string& key, const std::string& value)
{
    _params[key] = value;
}

const std::string& RuntimeOptions::getParam(const std::string& key) const
{
    if (_params.count(key) > 0)
        return (_params.at(key));
    else
        throw (DefaultEmptyParam);
}
