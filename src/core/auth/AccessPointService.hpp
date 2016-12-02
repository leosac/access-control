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
#include "core/audit/AuditFwd.hpp"
#include "tools/AssertCast.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"
#include <boost/type_index.hpp>
#include <map>
#include <type_traits>

namespace Leosac
{
namespace Auth
{

struct AccessPointBackend
{
    virtual json update(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
                        const json &req, IAccessPointPtr ap) = 0;
    virtual json create(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
                        const json &req) = 0;
    virtual json read(SecurityContext &sec_ctx, Audit::IAuditEntryPtr,
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
 * This service also manages runtime registered serializers that
 * target AccessPoint object.
 *
 *
 * @note This is a core service and therefore will always be
 * available to modules.
 */
class AccessPointService
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
                   "A backend for this serializer is already registered.");
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

    template <typename T>
    using SerializationCallable =
        std::function<json(const T &, const SecurityContext &sc)>;

    // fixme: Copy pasted from Audit's JSONService -- Consider writing
    // a templated ExtendableSerializerService or something.
    template <typename T>
    void register_serializer(SerializationCallable<T> fct)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        static_assert(std::is_base_of<Auth::IAccessPoint, T>::value,
                      "Trying to register a serializer for T, but T is not a child "
                      "of Auth::AccessPoint.");
        auto type_index = boost::typeindex::type_id<T>();
        ASSERT_LOG(serializers_.count(type_index) == 0,
                   "Already got a serializer for this type of AccessPoint.");

        // This is an adapter that wraps the user function. This allows us to
        // store uniform function objects.
        auto wrapper = [fct](const Auth::IAccessPoint &base_audit,
                             const SecurityContext &sc) -> json {
            // This cast is safe, because we'll only pick this serializer when
            // the typeid of the base_audit match T.
            const T &audit = reinterpret_cast<const T &>(base_audit);

            // However, for extra safety.
            const T &extra_safety = assert_cast<const T &>(base_audit);
            ASSERT_LOG(std::addressof(audit) == std::addressof(extra_safety),
                       "Memory error.");
            return fct(audit, sc);
        };
        serializers_[type_index] = wrapper;
    }

    template <typename T>
    void unregister_serializer()
    {
        std::lock_guard<std::mutex> lg(mutex_);

        auto type_index = boost::typeindex::type_id<T>();
        serializers_.erase(type_index);
    }

    /**
     * Serialize an access point using the appropriate serializer for the
     * concrete type of the object.
     *
     * If no such serializer can be found, Leosac will assert.
     */
    json serialize(const Auth::IAccessPoint &, const SecurityContext &sc);

  private:
    mutable std::mutex mutex_;

    /**
     * The various backend for various type of AccessPoint.
     * Those are registered by modules.
     */
    std::map<std::string, AccessPointBackend *> backends_;
    std::map<boost::typeindex::type_index, SerializationCallable<Auth::IAccessPoint>>
        serializers_;
};
}
}
