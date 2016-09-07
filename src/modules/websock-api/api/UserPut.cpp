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

#include "UserPut.hpp"
#include "Exceptions.hpp"
#include "UserEvent_odb.h"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "tools/db/DBService.hpp"
#include <core/audit/UserEvent.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

UserPut::UserPut(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr UserPut::create(RequestContext ctx)
{
    auto instance = std::make_unique<UserPut>(ctx);

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

    auto is_self = [ptr = instance.get()](const json &req)
    {
        auto uid = req.at("user_id").get<Auth::UserId>();
        return ptr->ctx_.session->current_user_id() == uid;
    };

    instance->add_conditions_or(
        []() { throw MalformedMessage("No `attributes` subobject"); },
        has_json_attributes_object);

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_self);
    return std::move(instance);
}

json UserPut::process_impl(const json &req)
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
        Audit::UserEventPtr audit = std::make_shared<Audit::UserEvent>();
        audit->target_            = user;
        audit->set_parent(ctx_.audit);
        audit->event_mask_ |= Audit::EventType::USER_EDITED;

        user->firstname(
            extract_with_default(attributes, "firstname", user->firstname()));
        user->lastname(
            extract_with_default(attributes, "lastname", user->lastname()));
        user->email(extract_with_default(attributes, "email", user->email()));

        db->persist(audit);
        db->update(user);
    }
    else
        throw EntityNotFound(uid, "user");
    t.commit();
    return rep;
}
