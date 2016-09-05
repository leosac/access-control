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

#pragma once

#include "database.hpp"
#include "tools/db/db_fwd.hpp"
#include <odb/session.hxx>

namespace Leosac
{
namespace db
{
/**
 * Acts like an odb::transaction, with the exception that it will
 * becomes the active transaction at construction, and will retore
 * the previous (if any) transaction when it gets destroyed.
  */
class MultiplexedTransaction
{
  public:
    MultiplexedTransaction(odb::transaction_impl *impl);
    ~MultiplexedTransaction();

    /**
     * Commit the transaction.
     *
     * The call is forward to the real odb::transaction object.
     */
    void commit();

  private:
    bool had_previous_;
    std::unique_ptr<odb::transaction> transaction_;
    odb::transaction *previous_;
};
}
}
