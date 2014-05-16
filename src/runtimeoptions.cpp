/**
 * \file runtimeoptions.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief RuntimeOptions class implementation
 */

#include "runtimeoptions.hpp"

void RuntimeOptions::setFlag(Flag flag, bool value)
{
    _flags.set(flag, value);
}

bool RuntimeOptions::isSet(Flag flag) const
{
    return (_flags.test(flag));
}
