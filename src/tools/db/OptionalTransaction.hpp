/*
    Copyright (C) 2014-2016 Leosac

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

#include "database.hpp"
#include "tools/db/db_fwd.hpp"
#include <odb/transaction.hxx>

namespace Leosac
{
namespace db
{
/**
 * An optional transaction is an object that behave like an
 * odb::transaction if there is no currently active transaction.
 *
 * If a transaction is already in progress, this object actually
 * does nothing, and calling commit() on it will do nothing.
 *
 *
 * The goal of this object is simple:
 *     + That the same code can either run independently (ie manages its
 *     own transaction, and commit() it) or rely on the caller's transaction
 *     for operation (ie, persists/update its objects, but let the caller commit()).
 */
class OptionalTransaction
{
  public:
    OptionalTransaction(odb::transaction_impl *impl);
    ~OptionalTransaction();

    /**
     * Commit the transaction, if there was no currently active transaction
     * at the time of this object's creation.
     */
    void commit();

    /**
     * Retrieve the odb transaction object.
     */
    odb::transaction *get();

  private:
    std::unique_ptr<odb::transaction> transaction_;
};
}
}
