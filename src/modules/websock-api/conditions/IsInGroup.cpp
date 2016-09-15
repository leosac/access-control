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

#include "IsInGroup.hpp"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "exception/leosacexception.hpp"
#include "tools/db/database.hpp"
#include "tools/log.hpp"
#include <tools/db/MultiplexedSession.hpp>
#include <tools/db/MultiplexedTransaction.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;
using namespace Leosac::Module::WebSockAPI::Conditions;

IsInGroup::IsInGroup(RequestContext ctx, Auth::GroupId group)
    : ConditionBase(ctx)
    , group_id_(group)
    , min_rank_(Auth::GroupRank::MEMBER)
{
}

IsInGroup::IsInGroup(RequestContext ctx, Auth::GroupId group,
                     Auth::GroupRank min_rank)
    : ConditionBase(ctx)
    , group_id_(group)
    , min_rank_(min_rank)
{
}


bool IsInGroup::operator()()
{
    auto user = ctx_.session->current_user();
    if (!user)
        return false;

    // Get a fresh copy of the User object from the database.
    // Then check group membership.
    {
        using query = odb::query<Auth::User>;
        DBPtr db    = ctx_.dbsrv->db();
        db::MultiplexedTransaction t(db->begin());

        Auth::UserPtr fresh_user =
            db->query_one<Auth::User>(query::id == user->id());
        if (!fresh_user)
        {
            ERROR("Failed to grab a fresh copy of the current user object.");
            throw LEOSACException(BUILD_STR(
                "Failed to grab a copy of user "
                << user->id() << " while checking IsInGroup (group: " << group_id_));
        }
        for (const auto &membership : fresh_user->group_memberships())
        {
            if (membership->group().object_id() == group_id_ &&
                membership->rank() >= min_rank_)
            {
                return true;
            }
        }
    }
    return false;
}
