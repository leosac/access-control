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

#include "api.hpp"
#include "../WSServer.hpp"
#include "core/CoreUtils.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/kernel.hpp"
#include "odb_gen/LogEntry_odb.h"
#include "odb_gen/LogEntry_odb_mysql.h"
#include "odb_gen/LogEntry_odb_sqlite.h"
#include "tools/db/database.hpp"
#include "tools/leosac.hpp"
#include <boost/date_time/posix_time/conversion.hpp>
#include <odb/mysql/database.hxx>
#include <odb/sqlite/database.hxx>
#include <tools/db/LogEntry.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

API::API(WSServer &server)
    : server_(server)
    , auth_status_(AuthStatus::NONE)
{
}

API::json API::get_leosac_version(const json &)
{
    json ret;
    ret["version"] = getVersionString();
    return ret;
}

API::json API::create_auth_token(const API::json &req)
{
    json rep;

    if (auth_status_ != AuthStatus::NONE)
    {
        rep["status"]  = -2;
        rep["message"] = "Already logged in";
    }
    else
    {
        std::string username = req.at("username");
        std::string password = req.at("password");

        ::Leosac::Auth::UserId uid;
        auto token = server_.auth().generate_token(username, password, uid);
        if (!token.empty())
        {
            rep["status"]  = 0;
            rep["user_id"] = uid;
            rep["token"]   = token;
            auth_status_   = AuthStatus::LOGGED_IN;
        }
        else
        {
            rep["status"] = -1;
        }
    }
    return rep;
}

API::json API::authenticate_with_token(const API::json &req)
{
    json rep;

    if (auth_status_ != AuthStatus::NONE)
    {
        rep["status"]  = -2;
        rep["message"] = "Already logged in";
    }
    else
    {
        std::string user_id;
        if (server_.auth().authenticate(req.at("token"), user_id))
        {
            rep["status"]       = 0;
            rep["user_id"]      = user_id;
            rep["username"]     = "lama"; // todo fix
            auth_status_        = AuthStatus::LOGGED_IN;
            current_auth_token_ = req.at("token");
        }
        else
        {
            rep["status"] = -1;
        }
    }
    return rep;
}

API::json API::logout(const API::json &)
{
    auth_status_ = AuthStatus::NONE;
    server_.auth().invalidate_token(current_auth_token_);
    current_auth_token_ = "";
    return {};
}

API::json API::system_overview(const API::json &req)
{
    json rep;
    auto core_api = server_.core_utils()->core_api();

    rep["instance_name"]  = core_api.instance_name();
    rep["config_version"] = core_api.config_version();
    rep["uptime"]         = core_api.uptime();
    rep["modules"]        = core_api.modules_names();

    return rep;
}

API::json API::get_logs(const json &req)
{
    json rep;

    using query    = odb::query<Tools::LogEntry>;
    using sl_query = odb::sqlite::query<Tools::LogEntry>;
    using my_query = odb::mysql::query<Tools::LogEntry>;
    using result   = odb::result<Tools::LogEntry>;
    DBPtr db       = server_.core_utils()->database();
    if (db)
    {
        rep["data"] = {};
        odb::transaction t(db->begin());

        int p  = extract_with_default(req, "p", 0);   // page
        int ps = extract_with_default(req, "ps", 20); // page size
        if (ps <= 0)
            ps               = 1;
        int offset           = p * ps;
        std::string sort     = extract_with_default(req, "sort", "desc");
        std::string order_by = sort == "asc" ? "ASC" : "DESC";
        query base_query;
        result base_result;

        // LIMIT needs to be database specific.
        if (db->id() == odb::database_id::id_sqlite)
        {
            auto sl_db = std::static_pointer_cast<odb::sqlite::database>(db);
            odb::sqlite::query<Tools::LogEntry> sl_q(
                "ORDER BY" + query::id + order_by + "LIMIT" + sl_query::_val(ps) +
                "OFFSET" + sl_query::_val(offset));
            base_result = sl_db->query<Tools::LogEntry>(sl_q);
        }
        else if (db->id() == odb::database_id::id_mysql)
        {
            auto my_db = std::static_pointer_cast<odb::mysql::database>(db);
            odb::mysql::query<Tools::LogEntry> my_q(
                "ORDER BY" + query::id + order_by + "LIMIT" + my_query::_val(ps) +
                "OFFSET" + my_query::_val(offset));
            base_result = my_db->query<Tools::LogEntry>(my_q);
        }
        Tools::LogView view(db->query_value<Tools::LogView>());

        for (Tools::LogEntry &entry : base_result)
        {
            auto timestamp = boost::posix_time::to_time_t(entry.timestamp_);
            rep["data"].push_back(
                {{"id", entry.id_},
                 {"type", "log-message"},
                 {"attributes",
                  {{"message", entry.msg_}, {"timestamp", timestamp}}}});
        }

        rep["meta"] = {
            {"total", view.count}, {"last", view.count / ps}, {"first", 0},
        };
        rep["status"] = 0;
    }
    else
    {
        rep["status"] = -1;
    }
    return rep;
}

bool API::allowed(const std::string &cmd)
{
    if (cmd == "get_leosac_version" || cmd == "create_auth_token" ||
        cmd == "authenticate_with_token")
        return true;
    return auth_status_ == AuthStatus::LOGGED_IN;
}
