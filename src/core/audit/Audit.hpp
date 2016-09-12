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

namespace Leosac
{
/**
 * The Audit namespace provides classes and facilities to keep track
 * of what's happening on the Leosac deamon.
 *
 * Through this collection of classes, Leosac is able to keep an *Audit Trail*
 * of what has happened. This can be rather useful, either to generate differential
 * update wrt configuration, or simply from a security point of view.
 *
 * @note This namespace is a bit particular, because its class are closely tied
 * to the underlying database and consistency is really important for those
 * objects.
 *
 * @note The Audit namespace and its class can be more intensive wrt the database
 * than other object. Read below for more info.
 *
 * The audit classes cannot be instanciated manually. The static methods of
 * Audit::Factory must be used instead. The factory ensure that certains step are
 * taken before handling you back a instance of an audit object.
 *
 *
 * **About the audit objects lifecycle**:
 *     1. Call Audit::Factory::UserEvent(...) or any other valid factory method.
 *     2. The factory method will instanciate, then persist (in an independant
 *        database transaction) the object. Calling `finalized()` will return false.
 *     3. You have the object. Call various setter to populate it.
 *     4. In a database transaction, call `finalize()`. This will update the object
 *        state in the database. It is YOUR responsiblity to commit the transaction
 *        in which the call to `finalize()` happened.
 *
 */
namespace Audit
{
}
}

#include "core/audit/AuditFactory.hpp"
#include "core/audit/AuditFwd.hpp"
#include "core/audit/IUserEvent.hpp"
#include "core/audit/IWSAPICall.hpp"
