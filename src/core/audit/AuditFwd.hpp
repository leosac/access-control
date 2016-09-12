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

#include <cstddef>
#include <flagset.hpp>
#include <memory>

namespace Leosac
{
namespace Audit
{
using AuditEntryId = unsigned long;

class Factory;

class IAuditEntry;
using IAuditEntryPtr = std::shared_ptr<IAuditEntry>;

class IUserEvent;
using IUserEventPtr = std::shared_ptr<IUserEvent>;

class IWSAPICall;
using IWSAPICallPtr = std::shared_ptr<IWSAPICall>;

class AuditEntry;
using AuditEntryPtr  = std::shared_ptr<AuditEntry>;
using AuditEntryWPtr = std::weak_ptr<AuditEntry>;

class WSAPICall;
using WSAPICallUPtr = std::unique_ptr<WSAPICall>;
using WSAPICallPtr  = std::shared_ptr<WSAPICall>;

class UserEvent;
using UserEventPtr = std::shared_ptr<UserEvent>;

enum class EventType
{
    WSAPI_CALL,
    USER_CREATED,
    USER_DELETED,
    USER_EDITED,
    USER_PASSWORD_CHANGED,
    /**
     * An attempt to change the password failed.
     */
    USER_PASSWORD_CHANGE_FAILURE,
    /**
     * A call to "user_get" websocket API has been made.
     */
    USER_GET,
    MEMBERSHIP_CREATED,
    MEMBERSHIP_DELETED,
    LAST__
};

using EventMask = FlagSet<EventType>;
}
}
