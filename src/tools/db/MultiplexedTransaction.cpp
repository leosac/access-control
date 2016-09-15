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

#include "MultiplexedTransaction.hpp"
#include <odb/transaction.hxx>

using namespace Leosac;
using namespace Leosac::db;


MultiplexedTransaction::MultiplexedTransaction(odb::transaction_impl *impl)
    : had_previous_(odb::transaction::has_current())
{
    if (had_previous_) // actually current
    {
        previous_ = &odb::transaction::current();
    }
    odb::transaction::reset_current();
    transaction_ = std::make_unique<odb::transaction>(impl);
}

MultiplexedTransaction::~MultiplexedTransaction()
{
    transaction_.reset();
    if (had_previous_)
    {
        odb::transaction::current(*previous_);
    }
}

void MultiplexedTransaction::commit()
{
    transaction_->commit();
}

void MultiplexedTransaction::rollback()
{
    transaction_->rollback();
}
