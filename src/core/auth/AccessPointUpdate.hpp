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

#include "core/update/Update.hpp"

namespace Leosac
{
namespace Auth
{
#pragma db object table("AccessPointUpdate")
class AccessPointUpdate : public ::Leosac::update::Update
{
  public:
    /**
     * Default constructor for the AccessPointUpdate object.
     * The update's `status_` defaults to ST_PENDING.
     */
    AccessPointUpdate();

    virtual ~AccessPointUpdate() = default;

    Auth::AccessPointId access_point_id() const;

    Auth::AccessPointLWPtr access_point() const;

    /**
     * Set the access point owning the updates.
     *
     * @note No database operation are performed here. It is
     * the caller responsibility to persist any change.
     */
    void access_point(Auth::AccessPointPtr ap);

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
