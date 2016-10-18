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

#include "WSAPICallSerializer.hpp"
#include "AuditSerializer.hpp"
#include "core/audit/IWSAPICall.hpp"

using namespace Leosac;

json WSAPICallJSONSerializer::serialize(const Audit::IWSAPICall &in,
                                        const SecurityContext &sc)
{
    // First we serialize the base ICredential structure.
    auto serialized = AuditJSONSerializer::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base audit serialization did something unexpected.");
    serialized["type"] = "audit-wsapicall-event";

    serialized["attributes"]["uuid"]            = in.uuid();
    serialized["attributes"]["method"]          = in.method();
    serialized["attributes"]["status-string"]   = in.status_string();
    serialized["attributes"]["status-code"]     = static_cast<int>(in.status_code());
    serialized["attributes"]["source-endpoint"] = in.source_endpoint();
    return serialized;
}
