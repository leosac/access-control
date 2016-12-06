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

#pragma once

#include "LeosacFwd.hpp"
#include "core/auth/IAccessPoint.hpp"

namespace Leosac
{
namespace Auth
{
#pragma db object optimistic polymorphic
class AccessPoint : public virtual IAccessPoint,
                    public std::enable_shared_from_this<AccessPoint>
{
  public:
    AccessPoint();
    AccessPointId id() const override;

    const std::string &alias() const override;

    void alias(const std::string &new_alias) override;

    const std::string &description() const override;

    void description(const std::string &dsc) override;

    void controller_module(const std::string &ctrl_mod) override;

    std::string controller_module() const override;

    IDoorPtr door() const override;

    DoorId door_id() const override;

    /**
     * Attach a new update object to the access-point.
     *
     * The inverse pointer of AccessPointUpdate object will be set
     * so that it points to this access-point.
     */
    void attach_update(AccessPointUpdatePtr);

  protected:
#pragma db id auto
    AccessPointId id_;
    std::string alias_;
    std::string description_;

#pragma db inverse(access_point_)
    std::weak_ptr<Door> door_;

    /**
     * Which module is responsible for this access point.
     */
    std::string controller_module_;


/**
 * The history of the updates performed against the access-point.
 */
#pragma db value_not_null
    std::vector<AccessPointUpdateLPtr> updates_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;

    friend class Leosac::TestAccess;

    /**
     * We are friend with Door so Door can set the inverse pointer (`door_`)
     * when the access-point is set on a door.
     */
    friend class Door;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/AccessPointUpdate.hpp"
#include "core/auth/Door.hpp"
#endif
