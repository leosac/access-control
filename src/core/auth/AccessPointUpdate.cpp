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

#include "AccessPointUpdate.hpp"
#include "AccessPoint_odb.h"

namespace Leosac
{
namespace Auth
{

AccessPointUpdate::AccessPointUpdate()
    : status_(ST_PENDING)
{
}

void AccessPointUpdate::set_checkpoint(Audit::AuditEntryPtr audit)
{
    checkpoint_.last(audit);
}


AccessPointUpdate::Status AccessPointUpdate::status() const
{
    return status_;
}

Audit::AuditEntryId AccessPointUpdate::get_checkpoint() const
{
    return checkpoint_.last_id();
}

Auth::AccessPointId AccessPointUpdate::access_point_id() const
{
    if (access_point_.lock())
        return access_point_.object_id();
    return 0;
}

void AccessPointUpdate::status(AccessPointUpdate::Status st)
{
    status_updated_at_ = std::chrono::system_clock::now();
    status_            = st;
}

AccessPointLWPtr AccessPointUpdate::access_point() const
{
    return access_point_;
}

const AccessPointUpdate::TimePoint &AccessPointUpdate::status_updated_at() const
{
    return status_updated_at_;
}

void AccessPointUpdate::access_point(Auth::AccessPointPtr ap)
{
    access_point_ = ap;
}
}
}
