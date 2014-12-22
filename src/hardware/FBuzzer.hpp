#pragma once

#include "FLED.hpp"

namespace Leosac
{
    namespace Hardware
    {
        /**
        * A Facade to a Buzzer object that you can turn on or off.
        * The interface is exactly the same as a LED, so this class is simply an alias.
        *
        * @see @ref Leosac::Hardware::FLED for details about this object behavior.
        */
        using FBuzzer = FLED;
    }
}
