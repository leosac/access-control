/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <zmqpp/message.hpp>

#pragma once

namespace Leosac
{
    namespace Auth
    {

        enum class SourceType : uint8_t
        {
            /**
            * This define message formatting for data source SIMPLE_WIEGAND.
            * You can find out more [here](@ref auth_data_type).
            */
            SIMPLE_WIEGAND = 1,
        };

        enum class AccessStatus : uint8_t
        {
            GRANTED = 1,
            DENIED = 2,
        };
    }
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::SourceType &t);

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::SourceType &t);

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::AccessStatus &st);

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::AccessStatus &st);

#include "AuthSourceBuilder.hpp"
#include "BaseAuthSource.hpp"
#include "WiegandCard.hpp"
