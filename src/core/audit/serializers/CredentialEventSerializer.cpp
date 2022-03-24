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

#include "CredentialEventSerializer.hpp"
#include "AuditSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/audit/ICredentialEvent.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Audit
{
namespace Serializer
{
json CredentialEventJSON::serialize(const Audit::ICredentialEvent &in,
                                    const SecurityContext &sc)
{
    auto serialized = AuditJSON::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base audit serialization did something unexpected.");
    serialized["type"] = "audit-credential-event";

    serialized["relationships"]["target"] = {
        {{"id", in.target_id()}, {"type", "credential"}}};

    if (sc.check_permission(SecurityContext::Action::AUDIT_READ_FULL))
    {
        serialized["attributes"]["before"] = in.before();
        serialized["attributes"]["after"]  = in.after();
    }

    return serialized;
}
}
}
}
