#include <zmqpp/message.hpp>

#pragma once

namespace Leosac
{
    namespace Auth
    {

        enum class SourceType : uint32_t
        {
            /**
            * This define message formatting for data source SIMPLE_WIEGAND.
            * You can find out more [here](@ref auth_data_type).
            */
            SIMPLE_WIEGAND = 1,
        };

        enum class AccessStatus : uint32_t
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
