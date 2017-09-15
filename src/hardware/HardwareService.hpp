/*
    Copyright (C) 2014-2017 Leosac

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

namespace Leosac
{
namespace Hardware
{
/**
 * Database aware Hardware Service.
 *
 * @note This is a core service that requires a database
 * to function. Therefore it'll be available only if Leosac is
 * configured to use a database.
 */
class HardwareService
{
  public:
    explicit HardwareService(DBServicePtr dbservice);

  private:
    DBServicePtr dbservice_;
};
}
}
