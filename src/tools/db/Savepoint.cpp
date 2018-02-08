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

#include "Savepoint.hpp"
#include "tools/GenGuid.h"
#include <boost/algorithm/string.hpp>
#include <spdlog/fmt/fmt.h>

namespace Leosac
{
namespace db
{

/**
 * Replace "-" by "_" in uuid string.
 */
static std::string fixup_uuid(const std::string &uuid)
{
    return boost::replace_all_copy(uuid, "-", "_");
}

Savepoint::Savepoint(odb::database &db)
    : Savepoint(db, fixup_uuid(gen_uuid()))
{
}

Savepoint::Savepoint(odb::database &db, const std::string &name)
    : db_(db)
    , name_(name)
{
    db_.execute(fmt::format("SAVEPOINT {}", name_));
}

Savepoint::~Savepoint()
{
    release();
}

void Savepoint::release()
{
    if (released_)
        return;

    db_.execute(fmt::format("RELEASE SAVEPOINT {}", name_));
    released_ = true;
}

bool Savepoint::rollback_to()
{
    if (released_)
        return false;

    db_.execute(fmt::format("ROLLBACK TO SAVEPOINT {}", name_));
    return true;
}
}
}
