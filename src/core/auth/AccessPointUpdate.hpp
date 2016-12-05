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

#include "core/audit/AuditTracker.hpp"
#include "core/update/Update.hpp"

namespace Leosac
{
    namespace Auth
{
#pragma db object table("AccessPointUpdate")
class AccessPointUpdate : public update::Update
{
  public:
    /**
     * Default constructor for the AccessPointUpdate object.
     * The update's `status_` defaults to ST_PENDING.
     */
    AccessPointUpdate();

    enum Status
    {
        ST_PENDING      = 0,
        ST_ACKNOWLEDGED = 1,
        ST_CANCELLED    = 2
    };

    /**
     * Pass the last audit entry at the time of this update's
     * generation.
     */
    void set_checkpoint(Audit::AuditEntryPtr);

    Status status() const;
    void status(Status st);

    Audit::AuditEntryId get_checkpoint() const;

    Auth::AccessPointId access_point_id() const;

    Auth::AccessPointLWPtr access_point() const;

    /**
     * Set the access point owning the updates.
     *
     * @note No database operation are performed here. It is
     * the caller responsibility to persist any change.
     */
    void access_point(Auth::AccessPointPtr ap);

    const TimePoint &status_updated_at() const;

  private:

    /**
     * Tracks the audit entry that represents the point in time
     * of this update.
     */
    Audit::AuditTracker checkpoint_;

    enum Status status_;

/**
 * Last timepoint when status was updated.
 *
 * Normally we should only change status once.
 */
#pragma db type("TIMESTAMP")
    TimePoint status_updated_at_;

/**
 * The access point targeted by this update.
 */
#pragma db not_null
#pragma db inverse(updates_)
    Auth::AccessPointLWPtr access_point_;

    friend class odb::access;
};
}
}
