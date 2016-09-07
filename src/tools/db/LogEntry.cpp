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


#include "LogEntry.hpp"
#include "LogEntry_odb.h"
#include "LogEntry_odb_mysql.h"
#include "LogEntry_odb_sqlite.h"
#include "tools/db/database.hpp"
#include <odb/mysql/database.hxx>
#include <odb/sqlite/database.hxx>

using namespace Leosac;
using namespace Leosac::Tools;

LogEntry::LogEntry()
    : version_(0)
{
}

LogEntry::QueryResult LogEntry::retrieve(DBPtr database, int page_number,
                                         int page_size, bool order_asc)
{
    std::vector<LogEntry> entries;

    using Query       = odb::query<Tools::LogEntry>;
    using SQLiteQuery = odb::sqlite::query<Tools::LogEntry>;
    using MySQLQuery  = odb::mysql::query<Tools::LogEntry>;
    using Result      = odb::result<Tools::LogEntry>;
    if (database)
    {
        odb::transaction t(database->begin());

        int offset           = page_number * page_size;
        std::string order_by = order_asc ? "ASC" : "DESC";
        Result res;

        // LIMIT needs to be database specific.
        if (database->id() == odb::database_id::id_sqlite)
        {
            auto sl_db = std::static_pointer_cast<odb::sqlite::database>(database);
            odb::sqlite::query<Tools::LogEntry> sl_q(
                "ORDER BY" + Query::id + order_by + "LIMIT" +
                SQLiteQuery::_val(page_size) + "OFFSET" + SQLiteQuery::_val(offset));
            res = sl_db->query<Tools::LogEntry>(sl_q);
        }
        else if (database->id() == odb::database_id::id_mysql)
        {
            auto my_db = std::static_pointer_cast<odb::mysql::database>(database);
            odb::mysql::query<Tools::LogEntry> my_q(
                "ORDER BY" + Query::id + order_by + "LIMIT" +
                MySQLQuery::_val(page_size) + "OFFSET" + MySQLQuery::_val(offset));
            res = my_db->query<Tools::LogEntry>(my_q);
        }
        Tools::LogView view(database->query_value<Tools::LogView>());
        for (Tools::LogEntry &entry : res)
        {
            entries.push_back(entry);
        }

        return {.entries = entries,
                .total   = view.count,
                .last    = view.count / page_size,
                .first   = 0};
    }
    return {};
}
