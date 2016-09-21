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

#include "api/MembershipCRUD.hpp"
#include "Exceptions.hpp"
#include "UserGroupMembership_odb.h"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include <json.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

MembershipCRUD::MembershipCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr MembershipCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new MembershipCRUD(ctx));
    return instance;
}

json MembershipCRUD::create_impl(const json &req)
{
    throw LEOSACException("Not implemented.");
}

json MembershipCRUD::read_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::UserGroupMembership>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();

    Auth::UserGroupMembershipPtr membership =
        db->query_one<Auth::UserGroupMembership>(query::id == mid);
    if (membership)
    {
        auto timestamp      = boost::posix_time::to_time_t(membership->timestamp());
        rep["data"]         = {};
        rep["data"]["id"]   = membership->id();
        rep["data"]["type"] = "user-group-membership";
        rep["data"]["attributes"] = {{"rank", static_cast<int>(membership->rank())},
                                     {"timestamp", timestamp}};
        rep["data"]["relationships"]["user"] = {
            {"data", {{"id", membership->user().object_id()}, {"type", "user"}}}};
        rep["data"]["relationships"]["group"] = {
            {"data", {{"id", membership->group().object_id()}, {"type", "group"}}}};
    }
    else
        throw EntityNotFound(mid, "user-group-membership");
    t.commit();
    return rep;
}

json MembershipCRUD::update_impl(const json &req)
{
    throw LEOSACException("Not implemented.");
}

json MembershipCRUD::delete_impl(const json &req)
{
    throw LEOSACException("Not implemented.");
}

std::vector<CRUDResourceHandler::ActionActionParam>
MembershipCRUD::required_permission(CRUDResourceHandler::Verb verb,
                                    const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::ActionParam ap;

    SecurityContext::MembershipActionParam map;
    try
    {
        map.membership_id = req.at("membership_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        map.membership_id = 0;
    }
    ap.membership = map;

    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::MEMBERSHIP_READ, ap));
        break;
    }
    return ret;
}
