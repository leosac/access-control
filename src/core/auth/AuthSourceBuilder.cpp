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

#include <tools/log.hpp>
#include "Auth.hpp"
#include "core/auth/PINCode.hpp"

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
    else if (type_name == SourceType::WIEGAND_PIN)
        return create_wiegand_pin_4bits(source_name, msg);

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
    assert(msg && msg->remaining() == 2);

    std::string card_id;
    int bits;

    *msg >> card_id >> bits;

    INFO("Building an AuthSource object (WiegandCard): " << card_id << " with " <<
            bits << " significants bits.");
    BaseAuthSourcePtr auth_source(new WiegandCard(card_id, bits));
    auth_source->name(name);

    return auth_source;
}

IAuthenticationSourcePtr AuthSourceBuilder::create_wiegand_pin_4bits(const std::string &name, zmqpp::message *msg)
{
    // pin code only
    assert(msg && msg->remaining() == 1);

    std::string pin;
    *msg >> pin;

    INFO("Building an AuthSource object (Wiegand 4bits Pin): " << pin);
    BaseAuthSourcePtr auth_source(new PINCode(pin));
    auth_source->name(name);

    return auth_source;
}
