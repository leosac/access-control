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

#include "AuditEntry.hpp"
#include "IAccessPointEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IAccessPointEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class AccessPointEvent : virtual public IAccessPointEvent, public AuditEntry
{
  private:
    AccessPointEvent();

    friend class Factory;

    static std::shared_ptr<AccessPointEvent> create(const DBPtr &database,
                                                    Auth::IAccessPointPtr target_ap,
                                                    AuditEntryPtr parent);

  public:
    virtual ~AccessPointEvent() = default;

    static std::shared_ptr<AccessPointEvent> create_empty();

    virtual void target(Auth::IAccessPointPtr door) override;

    Auth::AccessPointId target_id() const override;

    virtual void before(const std::string &repr) override;

    virtual void after(const std::string &repr) override;

    const std::string &before() const override;

    const std::string &after() const override;

    std::string generate_description() const override;

  public:
    /**
     * Generate a short description for the targeted AP.
     */
    std::string generate_target_description() const;

#pragma db on_delete(set_null)
    Auth::AccessPointLWPtr target_;

    Auth::AccessPointId target_ap_id_;

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
#include "core/auth/AccessPoint.hpp"
#endif
