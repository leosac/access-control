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

#pragma once

#include "DatabaseTracer.hpp"
#include <cstddef>
#include <cstdint>
#include <odb/pgsql/tracer.hxx>

namespace Leosac
{
namespace db
{
/**
 * An implementation of odb::tracer that use the logging infrastructure
 * of Leosac.
 */
class PGSQLTracer : public DatabaseTracer
{
  public:
    /**
     * @param count_only Do not log request, only count them.
     */
    PGSQLTracer(bool count_only);
    virtual void execute(odb::connection &connection, const char *statement);

    virtual size_t count() const override;

  private:
    size_t count_;
    bool count_only_;
};
}
}
