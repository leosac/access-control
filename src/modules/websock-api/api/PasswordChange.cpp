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

#include "PasswordChange.hpp"
#include "Exceptions.hpp"
#include "User_odb.h"
#include "WSServer.hpp"
#include "api/APISession.hpp"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/UserEvent.hpp"
#include "exception/EntityNotFound.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

PasswordChange::PasswordChange(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr PasswordChange::create(RequestContext ctx)
{
    auto instance = std::make_unique<PasswordChange>(ctx);

    // Change self password, or is an administrator and
    // can change anyone's password.

    auto is_self = [ptr = instance.get()](const json &req)
    {
        auto uid = req.at("user_id").get<Auth::UserId>();
        return ptr->ctx_.session->current_user_id() == uid;
    };

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_self);
    return std::move(instance);
}

json PasswordChange::process_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::User>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uid          = req.at("user_id").get<Auth::UserId>();
    auto new_password = req.at("new_password").get<std::string>();

    Auth::UserPtr user = db->query_one<Auth::User>(query::id == uid);
    if (user)
    {
        using namespace FlagSetOperator;
        Audit::IUserEventPtr audit = Audit::Factory::UserEvent(db, user, ctx_.audit);

        if (uid == ctx_.session->current_user_id())
        {
            auto current_password = req.at("current_password").get<std::string>();
            // When changing our own password, we check the `current_password` field.
            if (!user->verify_password(current_password))
            {
                audit->event_mask(Audit::EventType::USER_PASSWORD_CHANGE_FAILURE);
                audit->finalize();
                t.commit();
                throw PermissionDenied("Invalid `current_password`.");
            }
        }
        audit->event_mask(Audit::EventType::USER_EDITED |
                          Audit::EventType::USER_PASSWORD_CHANGED);
        user->password(new_password);

        ctx_.server.clear_user_sessions(user, ctx_.session, false);
        audit->finalize();
        db->update(user);
    }
    else
        throw EntityNotFound(uid, "user");
    t.commit();
    return rep;
}
