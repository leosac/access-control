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

#include <odb/database.hxx>

namespace Leosac
{
namespace db
{

/**
 * Provide a lightweight abstraction around
 * the SAVEPOINT SQL concept.
 *
 * This class implements SAVEPOINT, RELEASE SAVEPOINT
 * and ROLLBACK TO SAVEPOINT.
 *
 * @note This class leverages RAII:
 *      * The Savepoint is created (database-wise) when the object is instancied.
 *      * Upon deletion the Savepoint is released.
 */
class Savepoint
{
  public:
    /**
     * Construct a Savepoint with a random name.
     */
    Savepoint(odb::database &db);

    /**
     * Construct a Savepoint with a given name.
     */
    Savepoint(odb::database &db, const std::string &name);


    Savepoint(const Savepoint &) = delete;
    Savepoint(Savepoint &&o)     = delete;

    ~Savepoint();

    /**
     * Release the Savepoint if it wasn't released already,
     * otherwise does nothing.
     */
    void release();

    /**
     * Rollback to the Savepoint, unless the Savepoint
     * was already released.
     *
     * @return true if rollback was performed, false otherwise.
     */
    bool rollback_to();

  private:
    odb::database &db_;
    bool released_{false};
    std::string name_;
};
}
}
