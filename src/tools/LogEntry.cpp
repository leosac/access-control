/*
    Copyright (C) 2014-2022 Leosac

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
#include "exception/leosacexception.hpp"
#include "tools/LogEntry_odb.h"
#include "tools/LogEntry_odb_pgsql.h"
#include "tools/LogEntry_odb_sqlite.h"
#include "tools/db/database.hpp"
#include <odb/pgsql/database.hxx>
#include <odb/sqlite/database.hxx>

using namespace Leosac;
using namespace Leosac::Tools;

LogEntry::LogEntry()
    : version_(0)
{
}
using Query  = odb::query<Tools::LogEntry>;
using Result = odb::result<Tools::LogEntry>;

static Result fetch_sqlite(DBPtr database, const std::string &order_by,
                           int page_size, int offset)
{
    using SQLiteQuery = odb::sqlite::query<Tools::LogEntry>;
    auto sl_db        = std::static_pointer_cast<odb::sqlite::database>(database);
    odb::sqlite::query<Tools::LogEntry> sl_q("ORDER BY" + Query::id + order_by +
                                             "LIMIT" + SQLiteQuery::_val(page_size) +
                                             "OFFSET" + SQLiteQuery::_val(offset));
    return sl_db->query<Tools::LogEntry>(sl_q);
}

static Result fetch_pgsql(DBPtr database, const std::string &order_by, int page_size,
                          int offset)
{
    using PGSQLQuery = odb::pgsql::query<Tools::LogEntry>;
    auto pg_db       = std::static_pointer_cast<odb::pgsql::database>(database);
    odb::pgsql::query<Tools::LogEntry> pg_q("ORDER BY" + Query::id + order_by +
                                            "LIMIT" + PGSQLQuery::_val(page_size) +
                                            "OFFSET" + PGSQLQuery::_val(offset));
    return pg_db->query<Tools::LogEntry>(pg_q);
}

LogEntry::QueryResult LogEntry::retrieve(DBPtr database, int page_number,
                                         int page_size, bool order_asc)
{
    std::vector<LogEntry> entries;

    if (database)
    {
        odb::transaction t(database->begin());

        int offset           = page_number * page_size;
        std::string order_by = order_asc ? "ASC" : "DESC";
        Result res;

        // LIMIT needs to be database specific.
        if (database->id() == odb::database_id::id_sqlite)
        {
            res = fetch_sqlite(database, order_by, page_size, offset);
        }
        else if (database->id() == odb::database_id::id_pgsql)
        {
            res = fetch_pgsql(database, order_by, page_size, offset);
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
