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

#include "api/search/DoorSearch.hpp"
#include "Door_odb.h"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/auth/Door.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

DoorSearch::DoorSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr DoorSearch::create(RequestContext ctx)
{
    return std::make_unique<DoorSearch>(ctx);
}

struct DoorComparator
{
    bool operator()(const Auth::Door &g1, const Auth::Door &g2)
    {
        ASSERT_LOG(g1.id(), "g1 has no id.");
        ASSERT_LOG(g2.id(), "g2 has no id.");
        return g1.id() < g2.id();
    }
};

json DoorSearch::process_impl(const json &req)
{
    json rep = json::array();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    using Query              = odb::query<Auth::Door>;
    std::string partial_name = req.at("partial_name");
    std::set<Auth::Door, DoorComparator> doors;

    // We want a case insensitive search. However, there is no portable
    // way to do this. So for now we'll rely on a naive, potentially slow
    // bruteforce-style implementation.
    // todo: fixme

    for (auto i = 0u; i < partial_name.length(); ++i)
    {
        auto partial_name_copy = partial_name;
        partial_name_copy[i]   = std::toupper(partial_name[i]);
        Query q(Query::alias.like("%" + partial_name_copy + "%"));
        auto results = db->query(q);
        for (const auto &door : results)
        {
            doors.insert(door);
        }
    }

    Query q(Query::alias.like("%" + partial_name + "%"));
    auto results = db->query(q);
    for (const auto &door : results)
    {
        doors.insert(door);
    }

    for (const auto &door : doors)
    {
        json result_json = {{"id", door.id()}, {"alias", door.alias()}};
        rep.push_back(result_json);
    }

    return rep;
}

std::vector<MethodHandler::ActionActionParam>
DoorSearch::required_permission(const json &) const
{
    std::vector<MethodHandler::ActionActionParam> perm_;
    SecurityContext::ActionParam ap{};

    perm_.push_back({SecurityContext::Action::DOOR_SEARCH, ap});
    return perm_;
}
