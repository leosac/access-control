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

#include "WebSockAPI.hpp"
#include "WSServer.hpp"
#include <tools/db/database.hpp>
#include <boost/filesystem.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WebSockAPIModule::WebSockAPIModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                               const boost::property_tree::ptree &cfg,
                                               CoreUtilsPtr utils) :
    BaseModule(ctx, pipe, cfg, utils)
{
    port_ = cfg.get<uint16_t>("module_config.port", 8976);
    init_database();
}

void WebSockAPIModule::run()
{
    WSServer srv(*this, database_);
    std::thread thread(std::bind(&WSServer::run, &srv, port_));

    while (is_running_)
    {
        reactor_.poll();
    }
    INFO("WEBSOCK WILL START STOPPING...");
    srv.start_shutdown();
    thread.join();
}

void WebSockAPIModule::init_database()
{
    // initialise database;
    INFO("WebSocketAPI module is initialising database.");

    auto db_path = config_.get<std::string>("db_path", "wsapi.sqlite");
    boost::filesystem::path path(db_path);

    if (path.is_relative())
    {
        path = boost::filesystem::current_path() /= path;
    }

    INFO("Absolute dabatase path: " << path.string());
    // Try to open the database, assuming it already exists.
    database_ = std::make_shared<odb::sqlite::database>(path.string(),
                                                        SQLITE_OPEN_READWRITE);
    try
    {
        // Will throw if database doesn't exist.
        database_->connection();
    }
    catch (const odb::database_exception &e)
    {
        // Create the database.
        INFO("Database doesn't seem to exist. Will create it.");
        database_ = std::make_shared<odb::sqlite::database>(path.string(),
                                                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        // Create database structure.
        {
            odb::connection_ptr c(database_->connection());

            c->execute("PRAGMA foreign_keys=OFF");

            odb::transaction t(c->begin());
            odb::schema_catalog::create_schema(*database_);
            t.commit();

            c->execute("PRAGMA foreign_keys=ON");
        }
    }
}

CoreUtilsPtr WebSockAPIModule::core_utils()
{
    return utils_;
}
