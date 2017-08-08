/*
    Copyright (C) 2014-2016 Leosac

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

#include "AccessOverview.hpp"
#include "core/auth/Door.hpp"
#include "core/auth/Door_odb.h"
#include "core/auth/User_odb.h"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

AccessOverview::AccessOverview(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr AccessOverview::create(RequestContext ctx)
{
    return std::make_unique<AccessOverview>(ctx);
}

json AccessOverview::process_impl(const json &)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    // todo: This probably doesn't scale very well...

    auto doors = db->query<Auth::Door>();

    // Since we'll be looping over users multiple time, we cannot use
    // an odb::result object.
    auto users_odb = db->query<Auth::User>();
    // So we'll have to convert this to a vector of User, instead of
    // odb::result::iterator.
    std::vector<Auth::UserPtr> users;
    for (auto itr_odb(users_odb.begin()); itr_odb != users_odb.end(); ++itr_odb)
        users.push_back(itr_odb.load());

    for (const auto &door : doors)
    {
        std::set<Auth::UserId> unique_user_ids;
        json door_info = {{"door_id", door.id()}, {"user_ids", json::array()}};
        for (const auto &lazy_mapping : door.lazy_mapping())
        {
            auto mapping = lazy_mapping.load();
            for (const auto &user_ptr : users)
            {
                // Check the std::set in case the user is already authorized to
                // access the door.
                if (unique_user_ids.count(user_ptr->id()))
                {
                    continue;
                }
                if (mapping->has_user_indirect(user_ptr))
                {
                    unique_user_ids.insert(user_ptr->id());
                }
            }
        }
        for (const auto &id : unique_user_ids)
            door_info["user_ids"].push_back(id);
        rep.push_back(door_info);
    }

    return rep;
}

std::vector<ActionActionParam>
AccessOverview::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::ACCESS_OVERVIEW, ap});
    return perm_;
}
