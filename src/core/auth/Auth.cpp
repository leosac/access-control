#include "Auth.hpp"

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint8_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint8_t>(t);
    return msg;
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint8_t>::value,
            "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    t = static_cast<Leosac::Auth::SourceType>(tmp);
    return msg;
}

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::AccessStatus &st)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::AccessStatus>::type, uint8_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint8_t>(st);
    return msg;
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::AccessStatus &st)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::AccessStatus>::type, uint8_t>::value,
            "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    st = static_cast<Leosac::Auth::AccessStatus>(tmp);
    return msg;
}
