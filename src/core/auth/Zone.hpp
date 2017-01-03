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
#include "core/auth/IZone.hpp"

namespace Leosac
{
namespace Auth
{
/**
 * A Zone is a container for doors and other zone.
 *
 * Its a way to regroup element together, either in a physical
 * or logical way.
*/
#pragma db object optimistic
class Zone : public virtual IZone
{
  public:
    Zone();
    virtual ZoneId id() const override;

    virtual std::string alias() const override;

    virtual std::string description() const override;

    virtual void alias(const std::string &alias) override;

    virtual void description(const std::string &desc) override;

    virtual Type type() const override;

    virtual void type(Type t) override;

    virtual std::vector<ZoneLPtr> children() const override;

    virtual std::vector<DoorLPtr> doors() const override;

    virtual void clear_children() override;

    virtual void clear_doors() override;

    virtual void add_door(DoorLPtr door) override;

    virtual void add_child(ZoneLPtr zone) override;

  protected:
#pragma db id auto
    ZoneId id_;

    std::string alias_;
    std::string description_;
    Type type_;

#pragma db value_not_null
    std::vector<ZoneLPtr> children_;

#pragma db value_not_null
    std::vector<DoorLPtr> doors_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;
    friend class Leosac::TestAccess;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Door.hpp"
#endif
