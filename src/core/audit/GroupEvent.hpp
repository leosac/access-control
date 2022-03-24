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
#include "IGroupEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IGroupEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class GroupEvent : virtual public IGroupEvent, public AuditEntry
{
  private:
    GroupEvent() = default;

    friend class Factory;

    static std::shared_ptr<GroupEvent>
    create(const DBPtr &database, Auth::GroupPtr target_group, AuditEntryPtr parent);

  public:
    virtual ~GroupEvent() = default;

    static std::shared_ptr<GroupEvent> create_empty();

    virtual void target(Auth::GroupPtr grp) override;

    virtual void before(const std::string &repr) override;

    virtual void after(const std::string &repr) override;

    Auth::GroupId target_id() const override;

    const std::string &before() const override;

    const std::string &after() const override;

    std::string generate_description() const override;

  private:
    /**
     * Generate a small json-string description about the targeted
     * group.
     */
    std::string generate_target_description() const;

#pragma db on_delete(set_null)
    Auth::GroupLWPtr target_;

    /**
     * This is equals to target_->id().
     *
     * The reason for this data duplication is to keep
     * a trail of which group this event used to refer to.
     */
    Auth::GroupId target_group_id_;

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
