/*
    Copyright (C) 2014-2015 Islog

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

#include <algorithm>
#include <odb/object-result.hxx>
#include <odb/session.hxx>
#include "core/auth/Interfaces/IUser.hpp"
#include "odb_gen/IUser_odb.h"
#include "APIAuth.hpp"
#include "tools/GenGuid.h"
#include "WSServer.hpp"
#include "tools/db/database.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

APIAuth::APIAuth(WSServer &srv) :
    server_(srv)
{
}

std::string APIAuth::generate_token(const std::string &username, const std::string &password,
                                    Auth::UserId &user_id) // todo fix me by creating a proper token struct
{
    using namespace odb;
    using namespace odb::core;
    using query = odb::query<Auth::IUser>;
    using result = odb::result<Auth::IUser>;
    {
        auto db = server_.db();
        transaction t(db->begin());

        Auth::IUserPtr user = db->query_one<Auth::IUser>(query::username == username);
        if (user && user->password() == password)
        {
            auto token = gen_uuid();
            tokens_[token] = user->id();
            user_id = user->id();
            return token;
        }
    }
    return "";
}

bool APIAuth::authenticate(const std::string &token, std::string &user_id) const
{
    auto it = tokens_.find(token);
    if (it != tokens_.end())
    {
        user_id = it->second;
        return true;
    }
    return false;
}

void APIAuth::invalidate_token(const std::string &token)
{
    auto itr = tokens_.find(token);
    if (itr != tokens_.end())
        tokens_.erase(itr);
}
