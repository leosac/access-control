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

#include "api/MembershipCRUD.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IUserGroupMembershipEvent.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "core/auth/UserGroupMembership_odb.h"
#include "core/auth/User_odb.h"
#include "core/auth/serializers/UserGroupMembershipSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"
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

boost::optional<json> MembershipCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto attributes = req.at("attributes");
    auto gid        = attributes.at("group_id").get<size_t>();
    auto uid        = attributes.at("user_id").get<size_t>();
    Auth::GroupRank rank =
        static_cast<Auth::GroupRank>(attributes.at("rank").get<size_t>());

    auto group = ctx_.dbsrv->find_group_by_id(gid, DBService::THROW_IF_NOT_FOUND);
    auto user  = ctx_.dbsrv->find_user_by_id(uid, DBService::THROW_IF_NOT_FOUND);
    auto audit =
        Audit::Factory::UserGroupMembershipEvent(db, group, user, ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_MEMBERSHIP_JOINED);

    if (group->member_has(user->id()))
    {
        throw LEOSACException(BUILD_STR("User " << user->username()
                                                << " is already in group "
                                                << group->name()));
    }

    auto membership = group->member_add(user, rank);
    db->update(group);
    audit->finalize();
    t.commit();
    rep["data"] = UserGroupMembershipJSONSerializer::serialize(*membership,
                                                               security_context());
    return rep;
}

boost::optional<json> MembershipCRUD::read_impl(const json &req)
{
    json rep;

    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();

    Auth::UserGroupMembershipPtr membership =
        ctx_.dbsrv->find_membership_by_id(mid, DBService::THROW_IF_NOT_FOUND);
    rep["data"] = UserGroupMembershipJSONSerializer::serialize(*membership,
                                                               security_context());
    t.commit();
    return rep;
}

boost::optional<json> MembershipCRUD::update_impl(const json &)
{
    throw LEOSACException("Not implemented.");
}

boost::optional<json> MembershipCRUD::delete_impl(const json &req)
{
    odb::transaction t(ctx_.dbsrv->db()->begin());
    auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();

    Auth::UserGroupMembershipPtr membership =
        ctx_.dbsrv->find_membership_by_id(mid, DBService::THROW_IF_NOT_FOUND);
    auto audit = Audit::Factory::UserGroupMembershipEvent(
        ctx_.dbsrv->db(), membership->group().load(), membership->user().load(),
        ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_MEMBERSHIP_LEFT);
    ctx_.dbsrv->db()->erase(membership);
    audit->finalize();
    t.commit();
    return json{};
}

std::vector<CRUDResourceHandler::ActionActionParam>
MembershipCRUD::required_permission(CRUDResourceHandler::Verb verb,
                                    const json &req) const
{
    using namespace JSONUtil;

    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::MembershipActionParam map;
    map.membership_id = extract_with_default(req, "membership_id", 0u);
    map.user_id       = extract_with_default(req, "user_id", 0u);
    map.group_id      = extract_with_default(req, "group_id", 0u);
    map.rank = static_cast<Auth::GroupRank>(extract_with_default(req, "rank", 0u));

    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::MEMBERSHIP_READ, map));
        break;
    case Verb::CREATE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::GROUP_MEMBERSHIP_JOINED, map));
        break;
    case Verb::DELETE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::GROUP_MEMBERSHIP_LEFT, map));
        break;
    case Verb::UPDATE:
        // No permission required as the call is not implemented.
        break;
    }
    return ret;
}
