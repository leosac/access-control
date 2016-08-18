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

#include "DatabaseLogSink.hpp"
#include <tools/db/database.hpp>
#include <tools/db/LogEntry.hpp>
#include <tools/DateTimeConverter.hpp>
#include "LogEntry_odb.h"
#include "GenGuid.h"

using namespace Leosac;
using namespace Leosac::Tools;

DatabaseLogSink::DatabaseLogSink(DBPtr database) :
    database_(database)
{
    std::cout << "ENABLING SQLITE LOGGER!" << std::endl;
    assert(database_);
    // Generate a "run id"
    run_id_ = Leosac::gen_uuid();
}

void DatabaseLogSink::log(const spdlog::details::log_msg &msg)
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
