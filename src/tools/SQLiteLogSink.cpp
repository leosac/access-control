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

#include "SQLiteLogSink.hpp"
#include <tools/db/database.hpp>
#include <tools/db/LogEntry.hpp>
#include <tools/DateTimeConverter.hpp>
#include "LogEntry_odb.h"
#include "GenGuid.h"

using namespace Leosac;
using namespace Leosac::Tools;

SQLiteLogSink::SQLiteLogSink(const std::string &db_path)
{
    // Generate a "run id"
    run_id_ = Leosac::gen_uuid();

    database_ = std::make_shared<odb::sqlite::database>(
        db_path, SQLITE_OPEN_READWRITE);
    try
    {
        // Will throw if database doesn't exist.
        database_->connection();
    }
    catch (const odb::database_exception &e)
    {
        // Create the database.
        database_ = std::make_shared<odb::sqlite::database>(db_path,
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

void SQLiteLogSink::log(const spdlog::details::log_msg &msg)
{
    LogEntry entry;

    entry.level_ = msg.level;
    entry.msg_ = msg.formatted.str();
    entry.thread_id_ = msg.thread_id;
    entry.timestamp_ = time_point_ptime(msg.time);
    entry.run_id_ = run_id_;

    {
        using namespace odb;
        using namespace odb::core;
        transaction t(database_->begin());

        database_->persist(entry);
        t.commit();
    }
}
