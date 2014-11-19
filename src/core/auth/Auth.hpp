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
            SIMPLE_WIEGAND = 1
        };
    }
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint32_t>::value,
            "Bad underlying type for enum");
    uint32_t tmp;
    msg >> tmp;
    t = static_cast<Leosac::Auth::SourceType>(tmp);
    return msg;
}

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint32_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint32_t>(t);
    return msg;
}

#include "AuthSourceBuilder.hpp"
#include "BaseAuthSource.hpp"
#include "WiegandCard.hpp"
