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

#include "api/GroupCRUD.hpp"
#include "Exceptions.hpp"
#include "Group_odb.h"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IGroupEvent.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/GroupSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

GroupCRUD::GroupCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr GroupCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new GroupCRUD(ctx));

    auto has_json_attributes_object = [ptr = instance.get()](const json &req)
    {
        try
        {
            return req.at("attributes").is_object();
        }
        catch (const std::out_of_range &e)
        {
            return false;
        }
    };
    // Update conditions
    instance->add_conditions_or(
        Verb::UPDATE, []() { throw MalformedMessage("No `attributes` subobject"); },
        has_json_attributes_object);

    // Create conditions
    instance->add_conditions_or(
        Verb::CREATE, []() { throw MalformedMessage("No `attributes` subobject"); },
        has_json_attributes_object);

    return instance;
}

json GroupCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    // Sanitize input
    Auth::GroupPtr new_group = std::make_shared<Auth::Group>();

    GroupJSONSerializer::unserialize(*new_group, req.at("attributes"),
                                     security_context());
    validate_and_unique(new_group);

    db->persist(new_group);

    auto audit = Audit::Factory::GroupEvent(db, new_group, ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_CREATED);
    audit->after(GroupJSONStringSerializer::serialize(
        *new_group, SystemSecurityContext::instance()));
    audit->finalize();

    // Add the current user to the group as administrator.
    auto audit_add_to_group = Audit::Factory::GroupEvent(db, new_group, ctx_.audit);
    audit_add_to_group->event_mask(Audit::EventType::GROUP_MEMBERSHIP_JOINED);
    audit_add_to_group->before(GroupJSONStringSerializer::serialize(
        *new_group, SystemSecurityContext::instance()));

    new_group->member_add(ctx_.session->current_user(), Auth::GroupRank::ADMIN);

    db->update(new_group);
    audit_add_to_group->after(GroupJSONStringSerializer::serialize(
        *new_group, SystemSecurityContext::instance()));
    audit_add_to_group->finalize();

    // Send the model back to the client, so it knows the ID.
    rep["data"] = GroupJSONSerializer::serialize(*new_group, security_context());
    t.commit();
    return rep;
}

json GroupCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Auth::Group>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gid = req.at("group_id").get<Auth::GroupId>();

    if (gid != 0)
    {
        auto group =
            ctx_.dbsrv->find_group_by_id(gid, DBService::THROW_IF_NOT_FOUND);
        rep["data"] = GroupJSONSerializer::serialize(*group, security_context());
    }
    else
    {
        Result result     = db->query<Auth::Group>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &group : result)
        {
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::GROUP_READ,
                    {.group = {.group_id = group.id()}}))
                rep["data"].push_back(
                    GroupJSONSerializer::serialize(group, security_context()));
        }
    }
    t.commit();
    return rep;
}

json GroupCRUD::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gid = req.at("group_id").get<Auth::GroupId>();

    auto grp   = ctx_.dbsrv->find_group_by_id(gid, DBService::THROW_IF_NOT_FOUND);
    auto audit = Audit::Factory::GroupEvent(db, grp, ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_UPDATED);

    GroupJSONSerializer::unserialize(*grp, req.at("attributes"), security_context());

    validate_and_unique(grp);
    db->update(grp);

    audit->finalize();
    rep["data"] = GroupJSONSerializer::serialize(*grp, security_context());
    t.commit();
    return rep;
}

json GroupCRUD::delete_impl(const json &req)
{
    auto gid = req.at("group_id").get<Auth::GroupId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto group = ctx_.dbsrv->find_group_by_id(gid, DBService::THROW_IF_NOT_FOUND);
    auto audit = Audit::Factory::GroupEvent(db, group, ctx_.audit);
    audit->event_mask(Audit::EventType::GROUP_DELETED);

    db->erase(group);
    t.commit();

    return {};
}

void GroupCRUD::validate_and_unique(Auth::GroupPtr grp)
{
    using Query = odb::query<Auth::Group>;

    Auth::GroupValidator::validate(*grp);
    auto grp_with_same_name =
        ctx_.dbsrv->db()->query_one<Auth::Group>(Query::name == grp->name());
    // Check that either the name is available, or the group under modification
    // owns the name already.
    if (grp_with_same_name && grp_with_same_name != grp)
    {
        throw ModelException(
            "data/attributes/name",
            BUILD_STR("A group named " << grp->name() << " already exists."));
    }
}

std::vector<CRUDResourceHandler::ActionActionParam>
GroupCRUD::required_permission(CRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::GroupActionParam gap;
    try
    {
        gap.group_id = req.at("group_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        gap.group_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::GROUP_READ, gap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::GROUP_CREATE, gap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::GROUP_UPDATE, gap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::GROUP_DELETE, gap));
        break;
    }
    return ret;
}
