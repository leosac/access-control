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

#include "APISession.hpp"
#include "Exceptions.hpp"
#include "Group_odb.h"
#include "Token_odb.h"
#include "UserGroupMembership_odb.h"
#include "User_odb.h"
#include "WSServer.hpp"
#include "core/CoreUtils.hpp"
#include "core/UserSecurityContext.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "core/kernel.hpp"
#include "tools/GenGuid.h"
#include "tools/LogEntry.hpp"
#include "tools/db/MultiplexedSession.hpp"
#include "tools/leosac.hpp"
#include <odb/mysql/database.hxx>
#include <odb/session.hxx>
#include <odb/sqlite/database.hxx>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

APISession::APISession(WSServer &server)
    : server_(server)
    , auth_status_(AuthStatus::NONE)
    , connection_identifier_(gen_uuid())
{
}

APISession::json APISession::get_leosac_version(const json &)
{
    json ret;
    ret["version"] = getVersionString();
    return ret;
}

APISession::json APISession::create_auth_token(const APISession::json &req)
{
    json rep;
    ASSERT_LOG(auth_status_ == AuthStatus::NONE, "Invalid auth status.");

    std::string username = req.at("username");
    std::string password = req.at("password");

    auto token = server_.auth().authenticate_credentials(username, password);

    if (token)
    {
        rep["status"]  = 0;
        rep["user_id"] = token->owner()->id();
        rep["token"]   = token->token();
        mark_authenticated(token);
    }
    else
    {
        rep["status"] = -1;
    }

    return rep;
}

APISession::json APISession::authenticate_with_token(const APISession::json &req)
{
    json rep;
    ASSERT_LOG(auth_status_ == AuthStatus::NONE, "Invalid auth status.");

    auto token = server_.auth().authenticate_token(req.at("token"));
    if (token)
    {
        rep["status"]   = 0;
        rep["user_id"]  = token->owner()->id();
        rep["username"] = token->owner()->username();
        mark_authenticated(token);
    }
    else
    {
        rep["status"] = -1;
    }

    return rep;
}

APISession::json APISession::logout(const APISession::json &)
{
    ASSERT_LOG(auth_status_ == AuthStatus::LOGGED_IN, "Invalid auth status");
    ASSERT_LOG(current_auth_token_, "Logout called, but user has no current token.");
    server_.auth().invalidate_token(current_auth_token_);
    clear_authentication();
    return {};
}

APISession::json APISession::system_overview(const APISession::json &)
{
    json rep;
    auto core_api = server_.core_utils()->core_api();

    rep["instance_name"]  = core_api.instance_name();
    rep["config_version"] = core_api.config_version();
    rep["uptime"]         = core_api.uptime();
    rep["modules"]        = core_api.modules_names();

    return rep;
}

bool APISession::allowed(const std::string &cmd)
{
    if (cmd == "get_leosac_version")
        return true;
    if (cmd == "create_auth_token" || cmd == "authenticate_with_token")
        return auth_status_ != AuthStatus::LOGGED_IN;
    return auth_status_ == AuthStatus::LOGGED_IN;
}

void APISession::hook_before_request()
{
    if (auth_status_ == AuthStatus::LOGGED_IN)
    {
        odb::core::transaction t(server_.db()->begin());
        db::MultiplexedSession s;
        // Reload token
        try
        {
            server_.db()->reload(current_auth_token_);
        }
        catch (const odb::object_changed &e)
        {
            // Token doesn't exist anymore.
            abort_session();
            throw SessionAborted(nullptr);
        }

        if (!current_auth_token_->is_valid())
        {
            abort_session();
            throw SessionAborted(current_auth_token_);
        }

        current_auth_token_->expire_in(std::chrono::minutes(20));
        server_.db()->update(*current_auth_token_);
        t.commit();
    }
}

void APISession::abort_session()
{
    auth_status_        = AuthStatus::NONE;
    current_auth_token_ = nullptr;
}

Auth::UserId APISession::current_user_id() const
{
    if (current_auth_token_)
        return current_auth_token_->owner()->id();
    return 0;
}

Auth::UserPtr APISession::current_user() const
{
    if (current_auth_token_)
        return current_auth_token_->owner();
    return nullptr;
}

Auth::TokenPtr APISession::current_token() const
{
    return current_auth_token_;
}

void APISession::mark_authenticated(Auth::TokenPtr token)
{
    auth_status_        = AuthStatus::LOGGED_IN;
    current_auth_token_ = token;
    security_ =
        std::make_unique<UserSecurityContext>(server_.dbsrv(), token->owner()->id());
}

void APISession::clear_authentication()
{
    auth_status_        = AuthStatus::NONE;
    current_auth_token_ = nullptr;
    security_           = nullptr;
}

SecurityContext &APISession::security_context() const
{
    static NullSecurityContext sc;
    if (security_)
        return *security_.get();
    return sc;
}

const std::string &APISession::connection_identifier() const
{
    return connection_identifier_;
}
