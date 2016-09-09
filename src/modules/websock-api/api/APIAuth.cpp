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
#include "core/auth/User.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/GenGuid.h"
#include "tools/db/database.hpp"
#include <algorithm>
#include <odb/object-result.hxx>
#include <odb/session.hxx>
#include <tools/db/MultiplexedSession.hpp>
#include <tools/db/MultiplexedTransaction.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

APIAuth::APIAuth(WSServer &srv)
    : server_(srv)
{
}

void APIAuth::invalidate_token(Auth::TokenPtr token) const
{
    using namespace odb;
    using namespace odb::core;
    db::MultiplexedSession s;
    db::MultiplexedTransaction t(server_.db()->begin());
    auto t2 = server_.db()->load<Auth::Token>(token->token());
    server_.db()->erase(t2);
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

        Auth::UserPtr user = db->query_one<Auth::User>(query::username == username);
        if (user && user->password() == password)
        {
            // Create new token.
            auto token = std::make_shared<Auth::Token>(gen_uuid(), user);
            // Valid for 20m
            token->expire_in(std::chrono::minutes(20));
            db->persist(*token);
            t.commit();
            return token;
        }
    }
    return nullptr;
}
