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
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"
#include "tools/registry/Registry.hpp"
#include <boost/type_index.hpp>

namespace Leosac
{

/**
 * A class that manages services.
 *
 * The ServiceRegistry object provides facilities to manage
 * services. It can perform service registration, unregistration, retrieving.
 *
 * A service is a C++ object that implement a given interface. A limitation
 * of the ServiceRegistry is that it can only store one service instance per
 * interface type.
 *
 *
 * Important notes regarding object lifecycle:
 *    + `register_service()` takes a naked pointer. It is the responsibility of the
 *       caller to make sure that the service object will stay alive
 *       at least until a succesful call to `unregister_service()` happens.
 *    + The ServiceRegistry implements reference counting with regards to service
 *      object it hands over to client.
 *    + `unregister_service()` will fail if the service is currently being used by
 *       someone. "Being used" means holding a shared_ptr<> to the service instance,
 *       as returned by `get_service()`).
 */
class ServiceRegistry
{

    struct RegistrationInfo
    {
        boost::typeindex::type_index service_interface_;
        void *srv_;
    };

  public:
    using RegistrationHandle = std::weak_ptr<void>;

    template <typename ServiceInterface>
    RegistrationHandle register_service(ServiceInterface *srv)
    {
        using RegistrationInfoSPtr = std::shared_ptr<RegistrationInfo>;
        std::lock_guard<std::mutex> lg(mutex_);

        auto type_index = boost::typeindex::type_id<ServiceInterface>();
        ASSERT_LOG(!services_.has(type_index),
                   "Already has a service registered for this interface: "
                       << type_index.pretty_name());

        RegistrationInfoSPtr registration = std::make_shared<RegistrationInfo>();
        registration->service_interface_  = type_index;
        registration->srv_                = srv;
        services_.set(type_index, registration);

        return registration;
    }

    bool unregister_service(RegistrationHandle h)
    {
        std::lock_guard<std::mutex> lg(mutex_);

        std::shared_ptr<void> registration = h.lock();
        if (registration)
        {
            std::shared_ptr<RegistrationInfo> registration_sptr =
                std::static_pointer_cast<RegistrationInfo>(registration);

            // We must check that the service is not currently in use. If that's the
            // case we must prevent the unregistration of the service.
            // The reason for this if simple: It would make sense for the service
            // object to be deleted by its owner after a successful
            // unregister_service() call.
            // However, is someone else holds a reference to it, ... SEGV

            // SPTR to service: the registry itself, our registration
            // and registration_sptr objects.
            if (registration_sptr.use_count() > 3)
            {
                // Someone else has a ref to this service.
                return false;
            }

            // Sanity check
            ASSERT_LOG(services_.has(registration_sptr->service_interface_),
                       "Trying to unregister a service using an invalid handle.");
            services_.erase(registration_sptr->service_interface_);
            return true;
        }
        return false;
    }

    /**
     * Unregister the service for the interface ServiceInterface.
     */
    template <typename ServiceInterface>
    bool unregister_service()
    {
        boost::typeindex::type_index idx =
            boost::typeindex::type_id<ServiceInterface>();
        if (services_.has(idx))
        {
            auto registration =
                services_.get<std::shared_ptr<RegistrationInfo>>(idx);

            // Count: One in the registry, and the registration object.
            if (registration.use_count() > 2)
            {
                // Someone is using the service. Cannot unregister.
                return false;
            }
            services_.erase(idx);
            return true;
        }
        return false;
    }

    /**
     * Retrieve the service instance implementing the ServiceInterface,
     * or nullptr if no such service was registered.
     */
    template <typename ServiceInterface>
    std::shared_ptr<ServiceInterface> get_service() const
    {
        using RegistrationInfoSPtr = std::shared_ptr<RegistrationInfo>;
        std::lock_guard<std::mutex> lg(mutex_);

        auto type_index = boost::typeindex::type_id<ServiceInterface>();
        if (services_.has(type_index))
        {
            auto registration = services_.get<RegistrationInfoSPtr>(type_index);
            ServiceInterface *srv_ptr =
                static_cast<ServiceInterface *>(registration->srv_);
            return std::shared_ptr<ServiceInterface>(registration, srv_ptr);
        }
        return nullptr;
    }

    /**
     * How many strong (shared) pointer points to the
     * service that provides ServiceInterface
     */
    template <typename ServiceInterface>
    long use_count() const
    {
        auto srv = get_service<ServiceInterface>();

        if (!srv)
        {
            return -1;
        }

        auto count = srv.use_count();
        ASSERT_LOG(count >= 2, "Hum, we are missing some shared_ptr...");

        // Don't count the pointer we just retrieved, nor the pointer
        // stored in the registry.
        return count - 2;
    }

  private:
    mutable std::mutex mutex_;
    Registry<boost::typeindex::type_index> services_;
};
}
