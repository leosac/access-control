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

#include "IAuditEntry.hpp"
#include "tools/ToolsFwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Audit interface to Schedule related events.
 */
class IScheduleEvent : virtual public IAuditEntry
{
  public:
    MAKE_VISITABLE();
    /**
     * Set the user that is targeted by the event.
     */
    virtual void target(Tools::ISchedulePtr schd) = 0;

    /**
     * An optional JSON representation of the object
     * **before** the event took place.
     */
    virtual void before(const std::string &repr) = 0;

    /**
     * An optional JSON representation of the object
     * **after** the event took place.
     */
    virtual void after(const std::string &repr) = 0;
};
}
}
