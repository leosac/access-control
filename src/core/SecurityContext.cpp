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

#include "core/SecurityContext.hpp"

using namespace Leosac;

SecurityContext::SecurityContext(DBServicePtr dbsrv)
    : dbsrv_(dbsrv)
{
}

bool SecurityContext::check_permission(SecurityContext::Action,
                                       const ActionParam &) const
{
    return false;
}

SystemSecurityContext::SystemSecurityContext(DBServicePtr dbsrv)
    : SecurityContext(dbsrv)
{
}

bool SystemSecurityContext::check_permission(
    SecurityContext::Action, const SecurityContext::ActionParam &) const
{
    return true;
}

SecurityContext &SystemSecurityContext::instance()
{
    static SystemSecurityContext ssc(nullptr);
    return ssc;
}
