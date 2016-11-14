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

#include "core/SecurityContext.hpp"
#include "core/audit/AuditFwd.hpp"
#include "tools/AssertCast.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/registry/Registry.hpp"

namespace Leosac
{
namespace Audit
{
namespace Serializer
{

/**
 * This service manages runtime registered serializer that target AuditEntry object.
 *
 * We make no difference between the interface and the implementation because this
 * is a core service and there is no point in providing an alternative
 * implementation.
 *
 * Its goal is to hand over callable that can target a specific type of
 * AuditEntry.
 *
 * @note This service is for serialization extended audit entry (those provided by
 * modules) rather than the ones present in leosac's core.
 */
class JSONService
{
  public:
    /**
     * Perform the serialization of an audit entry using a runtime
     * registered serializer.
     *
     * @note This method will assert if no matching serializer are available.
     */
    json serialize(const Audit::IAuditEntry &, const SecurityContext &sc);

    ~JSONService();

    template <typename T>
    using SerializationCallable =
        std::function<json(const T &, const SecurityContext &sc)>;

    template <typename T>
    void register_serializer(SerializationCallable<T> fct)
    {
        static_assert(std::is_base_of<Audit::IAuditEntry, T>::value,
                      "Trying to register a serializer for T, but T is not a child "
                      "of IAuditEntry.");
        auto type_index = boost::typeindex::type_id<T>();
        ASSERT_LOG(serializers_.count(type_index) == 0,
                   "Already got a serializer for this type of audit entry.");

        // This is an adapter that wraps the user function. This allows us to
        // store uniform function objects.
        auto wrapper = [fct](const Audit::IAuditEntry &base_audit,
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
        auto type_index = boost::typeindex::type_id<T>();
        serializers_.erase(type_index);
    }

  private:
    std::map<boost::typeindex::type_index, SerializationCallable<Audit::IAuditEntry>>
        serializers_;
};
}
}
}
