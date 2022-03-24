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

#include "core/credentials/serializers/RFIDCardSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/IRFIDCard.hpp"
#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

json RFIDCardJSONSerializer::serialize(const IRFIDCard &in,
                                       const SecurityContext &sc)
{
    // First we serialize the base ICredential structure.
    auto serialized = CredentialJSONSerializer::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base credential serialization did something unexpected.");
    serialized["type"] = "rfid-card";

    serialized["attributes"]["nb-bits"] = in.nb_bits();
    serialized["attributes"]["card-id"] = in.card_id();
    return serialized;
}

void RFIDCardJSONSerializer::unserialize(Cred::IRFIDCard &out, const json &in,
                                         const SecurityContext &sc)
{
    CredentialJSONSerializer::unserialize(out, in, sc);

    using namespace JSONUtil;
    out.card_id(extract_with_default(in, "card-id", out.card_id()));
    out.nb_bits(extract_with_default(in, "nb-bits", out.nb_bits()));
}
