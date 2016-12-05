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

#include "UpdateFwd.hpp"
#include "tools/bs2.hpp"
#include <mutex>

namespace Leosac
{


namespace update
{


class UpdateBackend
{
  public:
    // check for update and create database entry if found.
    virtual void check_update() = 0;
};

/**
 * This service provides various update management
 * utilities.
 *
 * It's goal is to unify update management between various
 * modules. Modules can register their UpdateBackend object
 * against this service in order to participates in update detection.
 *
 * @note This service is considered a "core service" and is therefore
 * always available.
 *
 * @warning The update service requires that database be enabled.
 */
class UpdateService
{
  public:
    void check_update();

    /**
     * Retrieve the list of pending updates.
     */
    std::vector<IUpdatePtr> pending_updates();

    /**
     * Register a backend object.
     *
     * The backend is automatically deregistered when the `backend` object
     * is deleted.
     */
    void register_backend(UpdateBackendPtr backend);

  private:
    mutable std::mutex mutex_;

    using CheckUpdateT = boost::signals2::signal<void(void)>;
    CheckUpdateT check_update_sig_;
};
}
}
