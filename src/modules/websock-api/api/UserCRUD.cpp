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

#include "api/UserCRUD.hpp"
#include "Exceptions.hpp"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/UserJSONSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/db/DBService.hpp"

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
    return instance;
}

json UserCRUD::create_impl(const json &req)
{
    throw LEOSACException("Not implemented.");
}

json UserCRUD::read_impl(const json &req)
{
    json rep;

    using Query  = odb::query<Auth::User>;
    using Result = odb::result<Auth::User>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uid = req.at("user_id").get<Auth::UserId>();

    if (uid != 0)
    {
        Auth::UserPtr user = db->query_one<Auth::User>(Query::id == uid);
        if (user)
            rep["data"] = UserJSONSerializer::to_object(*user, security_context());
        else
            throw EntityNotFound(uid, "user");
    }
    else
    {
        // All users.
        Result result = db->query<Auth::User>();
        rep["data"]   = json::array();
        for (const auto &user : result)
        {
            rep["data"].push_back(
                UserJSONSerializer::to_object(user, security_context()));
        }
    }
    t.commit();
    return rep;
}

json UserCRUD::update_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::User>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uid        = req.at("user_id").get<Auth::UserId>();
    auto attributes = req.at("attributes");

    Auth::UserPtr user = db->query_one<Auth::User>(query::id == uid);
    if (user)
    {
        Audit::IUserEventPtr audit = Audit::Factory::UserEvent(db, user, ctx_.audit);
        audit->event_mask(Audit::EventType::USER_EDITED);
        audit->before(
            UserJSONSerializer::to_string(*user, SystemSecurityContext::instance()));

        user->firstname(
            extract_with_default(attributes, "firstname", user->firstname()));
        user->lastname(
            extract_with_default(attributes, "lastname", user->lastname()));
        user->email(extract_with_default(attributes, "email", user->email()));

        audit->after(
            UserJSONSerializer::to_string(*user, SystemSecurityContext::instance()));
        audit->finalize();
        db->update(user);
    }
    else
        throw EntityNotFound(uid, "user");
    t.commit();
    return rep;
}

json UserCRUD::delete_impl(const json &req)
{
    throw LEOSACException("Not implemented.");
}

std::vector<CRUDResourceHandler::ActionActionParam>
UserCRUD::required_permission(CRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::ActionParam ap;

    SecurityContext::UserActionParam uap;
    try
    {
        uap.user_id = req.at("user_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        uap.user_id = 0;
    }
    ap.user = uap;

    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_READ, ap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_CREATE, ap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_UPDATE, ap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::USER_DELETE, ap));
        break;
    }
    return ret;
}
