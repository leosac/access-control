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

#include "LeosacFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/auth/IDoor.hpp"

namespace Leosac
{
namespace Auth
{

/**
* Represent a door
*/
#pragma db object optimistic
class Door : public virtual IDoor
{
  public:
    Door();
    virtual DoorId id() const override;

    virtual std::string alias() const override;

    virtual std::string description() const override;

    virtual void alias(const std::string &alias) override;

    virtual void description(const std::string &desc) override;

    virtual IAccessPointPtr access_point() const override;

    virtual AccessPointId access_point_id() const override;

    virtual void access_point(IAccessPointPtr ptr) override;

    virtual std::vector<Tools::ScheduleMappingLWPtr> lazy_mapping() const override;

    /**
     * A ScheduleMapping object has added this door as part of its mapping.
     *
     * This method is called by the ScheduleMapping to give a chance to the
     * door to maintain its inverse vector.
     * @param sched_mapping
     */
    void schedule_mapping_added(const Tools::ScheduleMappingPtr &sched_mapping);

  protected:
#pragma db id auto
    DoorId id_;

    std::string alias_;
    std::string desc_;

    /**
     * The access point that controls the door.
     * May be null.
     */
    std::shared_ptr<AccessPoint> access_point_;

/**
 * ScheduleMapping that maps this door.
 */
#pragma db value_not_null inverse(doors_)
    std::vector<Tools::ScheduleMappingLWPtr> schedules_mapping_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;
    friend class Leosac::TestAccess;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/AccessPoint.hpp"
#endif
