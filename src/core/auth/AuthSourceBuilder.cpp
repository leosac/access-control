#include <tools/log.hpp>
#include "AuthSourceBuilder.hpp"
#include "BaseAuthSource.hpp"

using namespace Leosac::Auth;

IAuthenticationSourcePtr AuthSourceBuilder::create(zmqpp::message *msg)
{
    // Auth spec say at least 2 frames: source and type.
    assert(msg && msg->parts() >= 2);
    BaseAuthSourcePtr auth_source(new BaseAuthSource());

    std::string source_name;
    std::string type_name;
    *msg >> source_name >> type_name;

    if (extract_source_name(source_name, &source_name))
        auth_source->name(source_name);
    else
    {
        // invalid message.
        ERROR("Failed to extract source name: cannot construct the AuthSource object. " <<
                "Source name was {" << source_name << "}");
        assert(0);
    }

    return auth_source;
}

bool AuthSourceBuilder::extract_source_name(const std::string &input, std::string *output)
{
    assert(output);
    if (input.size() > 2 && input.substr(0, 2) == "S_")
    {
        *output = input.substr(2);
        return true;
    }
    return false;
}
