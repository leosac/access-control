/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "PinCodeSerializer.hpp"
#include "RFIDCardSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/ICredential.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

// JSON

json PolymorphicCredentialJSONSerializer::serialize(const ICredential &in,
                                                    const SecurityContext &sc)
{
    HelperSerialize h(sc);
    in.accept(h);
    return h.result_;
}

void PolymorphicCredentialJSONSerializer::unserialize(Cred::ICredential &out,
                                                      const json &in,
                                                      const SecurityContext &sc)
{
    HelperUnserialize h(sc, in);
    out.accept(h);
}

std::string
PolymorphicCredentialJSONSerializer::type_name(const Cred::ICredential &in)
{
    HelperSerialize h(SystemSecurityContext::instance());
    in.accept(h);
    ASSERT_LOG(h.result_.find("type") != h.result_.end(),
               "The serializer didn't set a type.");
    ASSERT_LOG(h.result_.at("type").is_string(), "Type is not a string.");
    return h.result_.at("type");
}


// HELPERS

PolymorphicCredentialJSONSerializer::HelperUnserialize::HelperUnserialize(
    const SecurityContext &sc, const json &payload)
    : security_context_(sc)
    , payload_(payload)
{
}

void PolymorphicCredentialJSONSerializer::HelperUnserialize::visit(
    Cred::IRFIDCard &t)
{
    RFIDCardJSONSerializer::unserialize(t, payload_, security_context_);
}

void PolymorphicCredentialJSONSerializer::HelperUnserialize::visit(IPinCode &t)
{
    PinCodeJSONSerializer::unserialize(t, payload_, security_context_);
}

PolymorphicCredentialJSONSerializer::HelperSerialize::HelperSerialize(
    const SecurityContext &sc)
    : security_context_(sc)
{
}

void PolymorphicCredentialJSONSerializer::HelperSerialize::visit(const IRFIDCard &t)
{
    result_ = RFIDCardJSONSerializer::serialize(t, security_context_);
}

void PolymorphicCredentialJSONSerializer::HelperSerialize::visit(const IPinCode &t)
{
    result_ = PinCodeJSONSerializer::serialize(t, security_context_);
}


// JSONString


std::string
PolymorphicCredentialJSONStringSerializer::serialize(const Cred::ICredential &in,
                                                     const SecurityContext &sc)
{
    return PolymorphicCredentialJSONSerializer::serialize(in, sc).dump(4);
}
