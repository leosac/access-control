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

#include "GroupDelete.hpp"
#include "Exceptions.hpp"
#include "Group_odb.h"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "conditions/IsInGroup.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IGroupEvent.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

GroupDelete::GroupDelete(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr GroupDelete::create(RequestContext ctx)
{
    auto instance = std::make_unique<GroupDelete>(ctx);

    auto is_group_admin = [ ctx = ctx, ptr = instance.get() ](const json &req)
    {
        using namespace Conditions;
        auto gid = req.at("group_id").get<Auth::GroupId>();
        return wrap(IsInGroup(ctx, gid, Auth::GroupRank::ADMIN))(req);
    };

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_group_admin);
    return std::move(instance);
}

json GroupDelete::process_impl(const json &req)
{
    auto gid = req.at("group_id").get<Auth::GroupId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto group = db->find<Auth::Group>(gid);
    if (!group)
        throw new EntityNotFound(gid, "group");

    auto audit = Audit::Factory::GroupEvent(db, group, ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_DELETED);

    db->erase(group);
    t.commit();

    return {};
}
