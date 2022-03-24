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

#include "PGSQLTracer.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::db;

PGSQLTracer::PGSQLTracer(bool count_only)
    : count_only_(count_only)
{
}

void PGSQLTracer::execute(odb::connection &, const char *statement)
{
    if (!count_only_)
        DEBUG("SQL: " << statement);
    ++count_;
}

size_t PGSQLTracer::count() const
{
    return count_;
}
