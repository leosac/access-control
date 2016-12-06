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
#include "core/SecurityContext.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/bs2.hpp"
#include "tools/serializers/ExtensibleSerializer.hpp"
#include <mutex>

namespace Leosac
{
namespace update
{

class UpdateBackend
{
  public:
    /**
     * Check for updates against arbitrary, module-owned object.
     *
     * The update returned by check update are supposed to be
     * transient. They are informative only and simply indicates
     * that an update is needed.
     *
     * Module specific websocket-handler should be called in order
     * to to perform other operation wrt updates.
     */
    virtual std::vector<update::IUpdatePtr> check_update() = 0;
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
    : public ExtensibleSerializer<json, IUpdate, const SecurityContext &>
{
  public:
    UpdateService();

    std::vector<update::IUpdatePtr> check_update();

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

    using CheckUpdateT =
        boost::signals2::signal<std::vector<update::IUpdatePtr>(void),
                                VectorAppenderCombiner<update::IUpdatePtr>>;

    CheckUpdateT check_update_sig_;
};
}
}
