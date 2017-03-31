/*
    Copyright (C) 2014-2016 Leosac

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

#include "AuthSourceBuilder.hpp"
#include "Auth.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "tools/enforce.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

namespace Leosac
{
namespace Auth
{
Cred::ICredentialPtr AuthSourceBuilder::create_simple_csn(const std::string &name,
                                                          zmqpp::message *msg)
{
    // card id
    assert(msg && msg->remaining() == 1);

    std::string card_id;

    *msg >> card_id;
    INFO("Building an AuthSource object (FAKE Wiegand Card):" << card_id);
    auto raw_csn = boost::replace_all_copy(card_id, ":", "");
    LEOSAC_ENFORCE(raw_csn.length() % 2 == 0, "CSN has invalid length.");

    auto c(std::make_shared<Cred::RFIDCard>());
    c->nb_bits(raw_csn.length() * 8);
    c->card_id(card_id);

    return c;
}

Cred::ICredentialPtr AuthSourceBuilder::create(zmqpp::message *msg)
{
    // Auth spec say at least 2 frames: source and type.
    assert(msg && msg->parts() >= 2);
    std::string source_name;
    SourceType type;
    *msg >> source_name >> type;

    bool ret = extract_source_name(source_name, &source_name);
    ASSERT_LOG(
        ret,
        "Failed to extract source name: cannot construct the AuthSource object. "
            << "Source name was {" << source_name << "}");

    if (type == SourceType::SIMPLE_WIEGAND)
    {
        return create_simple_wiegand(source_name, msg);
    }
    else if (type == SourceType::WIEGAND_PIN)
        return create_pincode(source_name, msg);
    else if (type == SourceType::WIEGAND_CARD_PIN)
        return create_wiegand_card_pin(source_name, msg);
    else if (type == SourceType::SIMPLE_CSN)
        return create_simple_csn(source_name, msg);
    LEOSAC_ENFORCE(0, "Unknown auth source type.");

    return nullptr;
}

bool AuthSourceBuilder::extract_source_name(const std::string &input,
                                            std::string *output) const
{
    assert(output);
    if (input.size() > 2 && input.substr(0, 2) == "S_")
    {
        *output = input.substr(2);
        return true;
    }
    return false;
}

Cred::ICredentialPtr
AuthSourceBuilder::create_simple_wiegand(const std::string &name,
                                         zmqpp::message *msg)
{
    // card id and number of bit shall be left.
    assert(msg && msg->remaining() == 2);

    std::string card_id;
    int bits;

    *msg >> card_id >> bits;

    INFO("Building a Credential object (RFIDCard): "
         << card_id << " with " << bits
         << " significant bits. Source name = " << name);
    auto c(std::make_shared<Cred::RFIDCard>());

    c->card_id(card_id);
    c->nb_bits(bits);
    return c;
}

Cred::ICredentialPtr AuthSourceBuilder::create_pincode(const std::string &name,
                                                       zmqpp::message *msg)
{
    // pin code only
    assert(msg && msg->remaining() == 1);

    std::string pin;
    *msg >> pin;

    INFO("Building a Credential object (PinCode): " << pin
                                                    << ". Source name = " << name);
    auto p = std::make_shared<Cred::PinCode>();
    p->pin_code(pin);
    return p;
}

Cred::ICredentialPtr
AuthSourceBuilder::create_wiegand_card_pin(const std::string &name,
                                           zmqpp::message *msg)
{
    // card id; nb bits; pin
    assert(msg && msg->remaining() == 3);

    std::string card_id;
    int bits;
    std::string pin_code;

    *msg >> card_id >> bits >> pin_code;
    INFO("Building a Credential object (RFIDCard + Pin):"
         << card_id << ", " << pin_code << ". Source name = " << name);

    auto c = std::make_shared<Cred::RFIDCard>();
    c->card_id(card_id);
    c->nb_bits(bits);

    auto p = std::make_shared<Cred::PinCode>();
    p->pin_code(pin_code);

    return std::make_shared<Cred::RFIDCardPin>(c, p);
}
}
}
