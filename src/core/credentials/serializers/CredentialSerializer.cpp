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

#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/ICredential.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

json CredentialJSONSerializer::serialize(const Cred::ICredential &in,
                                         const SecurityContext &sc)
{
    json serialized = {{"id", in.id()},
                       {"type", "credential"},
                       {"attributes",
                        {{"version", in.odb_version()},
                         {"alias", in.alias()},
                         {"description", in.description()}}}};

    if (in.owner_id())
    {
        serialized["relationships"]
                  ["owner"] = {{"data", {{"id", in.owner_id()}, {"type", "user"}}}};
    }
    return serialized;
}

void CredentialJSONSerializer::unserialize(Cred::ICredential &out, const json &in,
                                           const SecurityContext &sc)
{
    using namespace JSONUtil;
    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));

    // todo handle owner.
}
