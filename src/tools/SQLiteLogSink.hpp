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

#pragma once

#include <spdlog/sinks/sink.h>
#include <tools/db/db_fwd.hpp>

namespace Leosac
{
namespace Tools
{
/**
 * A custom sink that write LogEntry object
 * to a SQLite database.
 */
class SQLiteLogSink : public spdlog::sinks::sink
{
      public:

        /**
         * Construct a SQLite backed log sink.
         * @param database A non null pointer to a ODB database object.
         */
      SQLiteLogSink(DBPtr database);

        virtual void log(const spdlog::details::log_msg &msg) override;

        virtual void flush() override
        {
            // Noop as writing to a SQL database is transactional.
        }

        private:
        DBPtr database_;
        std::string run_id_;

};
}
}