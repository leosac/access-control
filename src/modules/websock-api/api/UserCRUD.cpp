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

#include "api/UserCRUD.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/auth/User.hpp"
#include "core/auth/User_odb.h"
#include "core/auth/serializers/UserSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

UserCRUD::UserCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr UserCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new UserCRUD(ctx));
    return instance;
}

boost::optional<json> UserCRUD::create_impl(const json &req)
{
    json rep;
    using Query = odb::query<Auth::User>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    json attributes = req.at("attributes");

    Auth::UserPtr new_user = std::make_shared<Auth::User>();
    new_user->username(attributes.at("username"));
    if (db->query_one<Auth::User>(Query::username == new_user->username()))
        throw ModelException("data/attributes/username",
                             BUILD_STR("The username " << new_user->username()
                                                       << " is already in use."));

    UserJSONSerializer::unserialize(*new_user, attributes, security_context());
    db->persist(new_user);

    Audit::IUserEventPtr audit = Audit::Factory::UserEvent(db, new_user, ctx_.audit);
    audit->event_mask(Audit::EventType::USER_CREATED);
    audit->after(UserJSONStringSerializer::serialize(
        *new_user, SystemSecurityContext::instance()));
    audit->finalize();
    t.commit();

    rep["data"] = UserJSONSerializer::serialize(*new_user, security_context());

    return rep;
}

boost::optional<json> UserCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Auth::User>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uid = req.at("user_id").get<Auth::UserId>();

    if (uid != 0)
    {
        Auth::UserPtr user =
            ctx_.dbsrv->find_user_by_id(uid, DBService::THROW_IF_NOT_FOUND);
        rep["data"] = UserJSONSerializer::serialize(*user, security_context());
    }
    else
    {
        // All users.
        Result result = db->query<Auth::User>();
        rep["data"]   = json::array();
        for (const auto &user : result)
        {
            rep["data"].push_back(
                UserJSONSerializer::serialize(user, security_context()));
        }
    }
    t.commit();
    return rep;
}

boost::optional<json> UserCRUD::update_impl(const json &req)
{
    json rep;

    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uid        = req.at("user_id").get<Auth::UserId>();
    auto attributes = req.at("attributes");

    Auth::UserPtr user =
        ctx_.dbsrv->find_user_by_id(uid, DBService::THROW_IF_NOT_FOUND);
    Audit::IUserEventPtr audit = Audit::Factory::UserEvent(db, user, ctx_.audit);
    audit->event_mask(Audit::EventType::USER_EDITED);
    audit->before(UserJSONStringSerializer::serialize(
        *user, SystemSecurityContext::instance()));

    bool enabled_status = user->validity().is_enabled();
    UserJSONSerializer::unserialize(*user, attributes, security_context());

    // prevent user from disabling themselves
    if (enabled_status && !user->validity().is_enabled() &&
        security_context().is_self(user->id()))
    {
        throw LEOSACException("You cannot disable your own user account.");
    }

    db->update(user);
    audit->after(UserJSONStringSerializer::serialize(
        *user, SystemSecurityContext::instance()));
    audit->finalize();
    t.commit();
    return rep;
}

boost::optional<json> UserCRUD::delete_impl(const json &)
{
    throw LEOSACException("Not implemented.");
}

std::vector<CRUDResourceHandler::ActionActionParam>
UserCRUD::required_permission(CRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;

    SecurityContext::UserActionParam uap;
    try
    {
        uap.user_id = req.at("user_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        uap.user_id = 0;
    }

    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_READ, uap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_CREATE, uap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_UPDATE, uap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_DELETE, uap));
        break;
    }
    return ret;
}
