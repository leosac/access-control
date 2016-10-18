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

#include "AuditGet.hpp"
#include "AuditEntry_odb.h"
#include "Exceptions.hpp"
#include "LogEntry_odb.h"
#include "api/APISession.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"
#include <core/audit/serializers/PolymorphicAuditSerializer.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

AuditGet::AuditGet(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr AuditGet::create(RequestContext ctx)
{
    return std::make_unique<AuditGet>(ctx);
}

json AuditGet::process_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    if (db)
    {
        using namespace Tools;
        using namespace JSONUtil;

        odb::transaction t(db->begin());
        auto ret       = db->query<Audit::AuditEntry>();
        rep["content"] = json::array();
        for (const auto &audit : ret)
        {
            json audit_json =
                PolymorphicAuditJSONSerializer::serialize(audit, security_context());
            rep["content"].push_back(audit_json);
        }
    }
    else
    {
        rep["status"] = -1;
    }
    return rep;
}

std::vector<MethodHandler::ActionActionParam>
AuditGet::required_permission(const json &) const
{
    std::vector<MethodHandler::ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::AUDIT_READ, ap});
    return perm_;
}
