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

#include "tools/db/db_fwd.hpp"
#include <odb/session.hxx>

namespace Leosac
{
namespace db
{
/**
 * Acts like an odb::session, with the exception that it will
 * save the current active session (if any) and restore it
 * when this MultiplexedSession ends.
 *
 * This object is useful to create new session without having to worry
 * about the currently active one.
 *
 * @see MultiplexedTransaction.
 */
class MultiplexedSession
{
  public:
    MultiplexedSession();
    ~MultiplexedSession();

  private:
    std::unique_ptr<odb::session> session_;
    odb::session *previous_;
};
}
}
