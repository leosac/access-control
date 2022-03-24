/*
    Copyright (C) 2014-2022 Leosac

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
#include "core/audit/AuditFwd.hpp"
#include "tools/AssertCast.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"
#include "tools/serializers/ExtensibleSerializer.hpp"
#include <boost/type_index.hpp>
#include <map>
#include <type_traits>

namespace Leosac
{
namespace Auth
{

/**
 * An AccessPointBackend object is used for module to provide
 * implementation for access point CRUD operations.
 *
 * An AccessPointBackend is mapped to one module, by name.
 */
struct AccessPointBackend
{
    virtual json update(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
                        const json &req, IAccessPointPtr ap) = 0;

    virtual json create(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
                        const json &req) = 0;

    virtual json erase(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
                       const json &req, IAccessPointPtr ap) = 0;
};

/**
 * This service lets various AccessPoint backend register
 * and provide implementation to use by the AccessPointCRUD object.
 *
 * The registration is done by specifying the name (string) of
 * the controller module for the type of AccessPoint.
 *
 * @note This service also manages runtime registered serializers that
 * target AccessPoint objects.
 *
 * @note This is a core service and therefore will always be
 * available to modules.
 *
 */
class AccessPointService
    : public ExtensibleSerializer<json, Auth::IAccessPoint, const SecurityContext &>
{
  public:
    /**
     * Register an AccessPointBackend for `controller_module`.
     *
     * The backend object MUST stay alive until a corresponding
     * call to unregister_backend() is made.
     */
    void register_backend(const std::string &controller_module,
                          AccessPointBackend *backend)
    {
        std::lock_guard<std::mutex> lg(mutex_);

        ASSERT_LOG(backends_.count(controller_module) == 0,
                   "A backend for this module is already registered.");
        backends_[controller_module] = backend;
    }

    void unregister_backend(const std::string &controller_module)
    {
        std::lock_guard<std::mutex> lg(mutex_);

        backends_.erase(controller_module);
    }

    /**
     * Get the AccessPointBackend that supports the given
     * `controller_module`.
     * @return nullptr if none found.
     */
    AccessPointBackend *get_backend(const std::string &controller_module);

  private:
    mutable std::mutex mutex_;

    /**
     * The various backend for various type of AccessPoint.
     * Those are registered by modules.
     */
    std::map<std::string, AccessPointBackend *> backends_;
};
}
}
