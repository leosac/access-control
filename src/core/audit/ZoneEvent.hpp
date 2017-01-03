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

#include "AuditEntry.hpp"
#include "IZoneEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IZoneEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class ZoneEvent : virtual public IZoneEvent, public AuditEntry
{
  private:
    ZoneEvent();

    friend class Factory;

    static std::shared_ptr<ZoneEvent>
    create(const DBPtr &database, Auth::IZonePtr target_zone, AuditEntryPtr parent);

  public:
    virtual ~ZoneEvent() = default;

    static std::shared_ptr<ZoneEvent> create_empty();

    virtual void target(Auth::IZonePtr zone) override;

    virtual Auth::ZoneId target_id() const override;

    virtual void before(const std::string &repr) override;

    virtual void after(const std::string &repr) override;

    virtual const std::string &before() const override;

    virtual const std::string &after() const override;

    virtual std::string generate_description() const override;

  public:
    /**
     * Generate a short description for the targeted zone.
     */
    std::string generate_target_description() const;

#pragma db on_delete(set_null)
    Auth::ZoneLWPtr target_;

    Auth::ZoneId target_zone_id_;

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
#include "core/auth/Zone.hpp"
#endif
