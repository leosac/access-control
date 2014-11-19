#include <tools/log.hpp>
#include "Auth.hpp"

using namespace Leosac::Auth;

IAuthenticationSourcePtr AuthSourceBuilder::create(zmqpp::message *msg)
{
    // Auth spec say at least 2 frames: source and type.
    assert(msg && msg->parts() >= 2);
    std::string source_name;
    SourceType type_name;
    *msg >> source_name >> type_name;

    if (!extract_source_name(source_name, &source_name))
    {
        // invalid message.
        ERROR("Failed to extract source name: cannot construct the AuthSource object. " <<
                "Source name was {" << source_name << "}");
        assert(0);
    }
    if (type_name == SourceType::SIMPLE_WIEGAND)
    {
        return create_simple_wiegand(source_name, msg);
    }
    assert(0);
}

bool AuthSourceBuilder::extract_source_name(const std::string &input, std::string *output) const
{
    assert(output);
    if (input.size() > 2 && input.substr(0, 2) == "S_")
    {
        *output = input.substr(2);
        return true;
    }
    return false;
}

IAuthenticationSourcePtr AuthSourceBuilder::create_simple_wiegand(
        const std::string &name,
        zmqpp::message *msg)
{
    // card id and number of bit shall be left.
   // assert(msg && msg->unread_parts() == 2);

    std::string card_id;
    int bits;

    *msg >> card_id >> bits;

    BaseAuthSourcePtr auth_source(new WiegandCard(card_id, bits));
    auth_source->name(name);

    return auth_source;
}
