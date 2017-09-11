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

#include "api/search/AccessPointSearch.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/auth/AccessPoint.hpp"
#include "core/auth/AccessPoint_odb.h"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

AccessPointSearch::AccessPointSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr AccessPointSearch::create(RequestContext ctx)
{
    return std::make_unique<AccessPointSearch>(ctx);
}

struct AccessPointComparator
{
    bool operator()(const Auth::AccessPoint &ap1, const Auth::AccessPoint &ap2)
    {
        ASSERT_LOG(ap1.id(), "g1 has no id.");
        ASSERT_LOG(ap2.id(), "g2 has no id.");
        return ap1.id() < ap2.id();
    }
};

json AccessPointSearch::process_impl(const json &req)
{
    json rep = json::array();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    using Query              = odb::query<Auth::AccessPoint>;
    std::string partial_name = req.at("partial_name");
    std::set<Auth::AccessPoint, AccessPointComparator> access_points;

    // todo fix me performance

    for (auto i = 0u; i < partial_name.length(); ++i)
    {
        auto partial_name_copy = partial_name;
        partial_name_copy[i]   = std::toupper(partial_name[i]);
        Query q(Query::alias.like("%" + partial_name_copy + "%"));
        auto results = db->query(q);
        for (const auto &ap : results)
        {
            access_points.insert(ap);
        }
    }

    Query q(Query::alias.like("%" + partial_name + "%"));
    auto results = db->query(q);
    for (const auto &ap : results)
    {
        access_points.insert(ap);
    }

    for (const auto &ap : access_points)
    {
        json result_json = {{"id", ap.id()}, {"alias", ap.alias()}};
        rep.push_back(result_json);
    }

    return rep;
}

std::vector<ActionActionParam>
AccessPointSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    perm_.push_back({SecurityContext::Action::ACCESS_POINT_SEARCH, {}});
    return perm_;
}
