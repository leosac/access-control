/*
    Copyright (C) 2014-2022 Leosac

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

#include "AuditEntry.hpp"
#include "IScheduleEvent.hpp"
#include "tools/ToolsFwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IScheduleEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class ScheduleEvent : virtual public IScheduleEvent, public AuditEntry
{
  private:
    ScheduleEvent() = default;

    friend class Factory;

    static std::shared_ptr<ScheduleEvent> create(const DBPtr &database,
                                                 Tools::ISchedulePtr target_sched,
                                                 AuditEntryPtr parent);

  public:
    /**
     * A factory function for usage in tests.
     */
    static std::shared_ptr<ScheduleEvent> create_empty();

    virtual ~ScheduleEvent() = default;

    virtual void target(Tools::ISchedulePtr sch) override;

    virtual void before(const std::string &repr) override;

    virtual void after(const std::string &repr) override;

    Tools::ScheduleId target_id() const override;

    const std::string &before() const override;

    const std::string &after() const override;

    std::string generate_description() const override;

  private:
    std::string generate_target_description() const;

#pragma db on_delete(set_null)
    Tools::ScheduleLWPtr target_;

    Tools::ScheduleId target_sched_id_;

    /**
     * Optional JSON dump of the object before the event took place.
     */
    std::string before_;

    /**
     * Optional JSON dump of the object after the event took place.
     */
    std::string after_;

    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "tools/Schedule.hpp"
#endif
