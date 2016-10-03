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
#include "User_odb.h"
#include "core/SecurityContext.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "core/credentials/ICredential.hpp"
#include "tools/GlobalRegistry.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

static std::string tp_to_str(const Auth::ValidityInfo::TimePoint &tp)
{
    std::stringstream ss;
    std::time_t dt_time_t = std::chrono::system_clock::to_time_t(tp);
    ss << std::put_time(std::localtime(&dt_time_t), "%FT%TZ");
    return ss.str();
}

json CredentialJSONSerializer::serialize(const Cred::ICredential &in,
                                         const SecurityContext &sc)
{
    json serialized = {{"id", in.id()},
                       {"type", "credential"},
                       {"attributes",
                        {
                            {"version", in.odb_version()},
                            {"alias", in.alias()},
                            {"description", in.description()},
                            {"validity-enabled", in.validity().is_enabled()},
                            {"validity-start", tp_to_str(in.validity().start())},
                            {"validity-end", tp_to_str(in.validity().end())},
                        }}};

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

    // Credential validity attributes
    auto validity = out.validity();
    validity.set_enabled(
        extract_with_default(in, "validity-enabled", validity.is_enabled()));

    validity.start(extract_with_default(
        in, "validity-start", std::chrono::system_clock::time_point::min()));
    validity.end(extract_with_default(in, "validity-end",
                                      std::chrono::system_clock::time_point::max()));
    out.validity(validity);


    // Owner
    Auth::UserId new_owner_id = extract_with_default(in, "owner_id", out.owner_id());
    if (new_owner_id != out.owner_id())
    {
        if (new_owner_id)
        {
            DBPtr dbptr = GlobalRegistry::get<DBPtr>(GlobalRegistry::DATABASE);
            Auth::UserLPtr new_owner(*dbptr, new_owner_id);
            out.owner(new_owner);
        }
        else
        {
            out.owner(nullptr);
        }
    }
}
