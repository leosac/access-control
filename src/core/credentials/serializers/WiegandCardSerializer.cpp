/*
    Copyright (C) 2014-2016 Islog

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

#include "core/credentials/serializers/WiegandCardSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/IWiegandCard.hpp"
#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

json WiegandCardJSONSerializer::serialize(const IWiegandCard &in,
                                          const SecurityContext &sc)
{
    // First we serialize the base ICredential structure.
    auto serialized = CredentialJSONSerializer::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base credential serialization did something unexpected.");
    serialized["type"] = "wiegand-card";

    serialized["attributes"]["nb-bits"] = in.nb_bits();
    serialized["attributes"]["card-id"] = in.card_id();
    return serialized;
}
