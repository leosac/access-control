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
#include <odb/callback.hxx>

namespace Leosac
{
namespace Auth
{

class ZoneValidator
{
  public:
    /**
     * Perform validation of the zone.
     *
     * The function will make sure that the zone do not
     * have 2 physical's zone as parent.
     *
     * @param zone_ids Set of zone ids that were iterated over
     * during the validation. This is to prevent cycle in parent/child relationship.
     */
    static void validate(const Zone &z, std::set<ZoneId> &zone_ids);

    /**
     * Validate that the integer value of the type is a correct
     * type for a zone.
     *
     * This is mostly useful when unserializing from an untrusted source.
     */
    static void validate_type(IZone::Type value);
};

/**
 * A Zone is a container for doors and other zone.
 *
 * This is the ODB-aware implementation.
 */
#pragma db object callback(validation_callback) optimistic
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

    /**
     * Callback function called by ODB before/after database
     * operation against a Zone object.
     *
     * This function will perform some validation wrt parent/child
     * relationship of the object.
     */
    void validation_callback(odb::callback_event e, odb::database &) const;

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

#pragma db value_not_null inverse(children_)
    std::vector<ZoneLWPtr> parents_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;
    friend class Leosac::TestAccess;
    friend class ZoneValidator;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Door.hpp"
#endif
