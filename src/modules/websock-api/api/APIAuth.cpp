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

#include "APIAuth.hpp"
#include "Token_odb.h"
#include "User_odb.h"
#include "WSServer.hpp"
#include "core/CoreAPI.hpp"
#include "core/CoreUtils.hpp"
#include "core/auth/User.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/GenGuid.h"
#include "tools/Mail.hpp"
#include "tools/db/MultiplexedSession.hpp"
#include "tools/db/MultiplexedTransaction.hpp"
#include "tools/db/database.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <odb/object-result.hxx>
#include <odb/session.hxx>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

APIAuth::APIAuth(WSServer &srv)
    : server_(srv)
{
}

void APIAuth::invalidate_token(Auth::TokenPtr token) const
{
    ASSERT_LOG(token, "nullptr passed when excepting non-null token.");

    using namespace odb;
    using namespace odb::core;
    odb::transaction t(server_.db()->begin());
    server_.db()->erase<Auth::Token>(token->id());
    t.commit();
}

Auth::TokenPtr APIAuth::authenticate_token(const std::string &token_str) const
{
    using namespace odb;
    using namespace odb::core;
    using query = odb::query<Auth::Token>;

    auto db = server_.db();
    transaction t(db->begin());
    db::MultiplexedSession s;

    Auth::TokenPtr token(db->query_one<Auth::Token>(query::token == token_str));
    if (token && token->is_valid())
    {
        enforce_user_enabled(*token->owner());
        token->expire_in(std::chrono::minutes(20));
        db->update(token);
        t.commit();
        return token;
    }
    return nullptr;
}

Auth::TokenPtr APIAuth::authenticate_credentials(const std::string &username,
                                                 const std::string &password) const
{
    using namespace odb;
    using namespace odb::core;
    using query = odb::query<Auth::User>;
    {
        auto db = server_.db();
        transaction t(db->begin());

        auto username_lowercase = boost::algorithm::to_lower_copy(username);
        Auth::UserPtr user =
            db->query_one<Auth::User>(query::username == username_lowercase);
        if (user && user->verify_password(password))
        {
            enforce_user_enabled(*user);
            // Create new token.
            auto token = std::make_shared<Auth::Token>(gen_uuid(), user);
            // Valid for 20m
            token->expire_in(std::chrono::minutes(20));
            db->persist(*token);
            t.commit();

            if (user->username() == "admin")
            {
                MailSender ms(server_.core_utils());
                MailInfo mail;
                mail.title = "Admin Connected";
                mail.body  = "The user `admin` logged in !";
                // todo make this configurable or something.
                // for now its just to see how MailSender can be used.
                // ms.send_to_admin(mail);
            }

            return token;
        }
    }
    return nullptr;
}

void APIAuth::enforce_user_enabled(const Auth::User &u) const
{
    const auto &validity = u.validity();
    if (!validity.is_enabled())
        throw LEOSACException(BUILD_STR("This user account is disabled."));
    if (!validity.is_in_range())
        throw LEOSACException(
            BUILD_STR("This user account is not currently active."));
}
