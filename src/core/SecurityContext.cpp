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
#include "exception/PermissionDenied.hpp"

using namespace Leosac;

SecurityContext::SecurityContext(DBServicePtr dbsrv)
    : dbsrv_(dbsrv)
{
}

bool SecurityContext::check_permission(SecurityContext::Action a,
                                       const ActionParam &ap) const
{
    return check_permission_impl(a, ap);
}

void SecurityContext::enforce_permission(
    SecurityContext::Action a, const SecurityContext::ActionParam &ap) const
{
    if (!check_permission(a, ap))
        throw PermissionDenied();
}

bool SecurityContext::check_permission(SecurityContext::Action a) const
{
    return check_permission(a, {});
}

void SecurityContext::enforce_permission(SecurityContext::Action a) const
{
    return enforce_permission(a, {});
}

SystemSecurityContext::SystemSecurityContext(DBServicePtr dbsrv)
    : SecurityContext(dbsrv)
{
}

bool SystemSecurityContext::check_permission_impl(
    SecurityContext::Action, const SecurityContext::ActionParam &) const
{
    return true;
}

SecurityContext &SystemSecurityContext::instance()
{
    static SystemSecurityContext ssc(nullptr);
    return ssc;
}

SecurityContext::GroupActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.group = *this;
    return result;
}

SecurityContext::UserActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.user = *this;
    return result;
}

SecurityContext::MembershipActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.membership = *this;
    return result;
}

SecurityContext::CredentialActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.cred = *this;
    return result;
}

SecurityContext::ScheduleActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.sched = *this;
    return result;
}

SecurityContext::DoorActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.door = *this;
    return result;
}

SecurityContext::AccessPointActionParam::operator ActionParam()
{
    SecurityContext::ActionParam result;
    result.access_point = *this;
    return result;
}


NullSecurityContext::NullSecurityContext()
    : SecurityContext(nullptr)
{
}

bool NullSecurityContext::check_permission_impl(
    SecurityContext::Action, const SecurityContext::ActionParam &) const
{
    return false;
}
