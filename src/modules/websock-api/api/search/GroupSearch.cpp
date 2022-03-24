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

#include "api/search/GroupSearch.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/Group_odb.h"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

GroupSearch::GroupSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr GroupSearch::create(RequestContext ctx)
{
    return std::make_unique<GroupSearch>(ctx);
}

struct GroupComparator
{
    bool operator()(const Auth::Group &g1, const Auth::Group &g2)
    {
        ASSERT_LOG(g1.id(), "g1 has no id.");
        ASSERT_LOG(g2.id(), "g2 has no id.");
        return g1.id() < g2.id();
    }
};

json GroupSearch::process_impl(const json &req)
{
    json rep = json::array();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    using Query              = odb::query<Auth::Group>;
    std::string partial_name = req.at("partial_name");
    std::set<Auth::Group, GroupComparator> groups;

    // We want a case insensitive search. However, there is no portable
    // way to do this. So for now we'll rely on a naive, potentially slow
    // bruteforce-style implementation.
    // todo: fixme

    for (auto i = 0u; i < partial_name.length(); ++i)
    {
        auto partial_name_copy = partial_name;
        partial_name_copy[i]   = std::toupper(partial_name[i]);
        Query q(Query::name.like("%" + partial_name_copy + "%"));
        auto results = db->query(q);
        for (const auto &group : results)
        {
            groups.insert(group);
        }
    }

    Query q(Query::name.like("%" + partial_name + "%"));
    auto results = db->query(q);
    for (const auto &group : results)
    {
        groups.insert(group);
    }

    for (const auto &group : groups)
    {
        json result_json = {{"id", group.id()}, {"name", group.name()}};
        rep.push_back(result_json);
    }

    return rep;
}

std::vector<ActionActionParam> GroupSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap{};

    perm_.push_back({SecurityContext::Action::GROUP_SEARCH, ap});
    return perm_;
}
