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
 *       at least until a successful call to `unregister_service()` happens.
 *    + The ServiceRegistry implements reference counting with regards to service
 *      object it hands over to client.
 *    + `unregister_service()` will fail if the service is currently being used by
 *       someone. "Being used" means holding a shared_ptr<> to the service instance,
 *       as returned by `get_service()`).
 */
class ServiceRegistry
{

    /**
     * An internal registration structure.
     *
     * Each registered services is mapped to a RegistrationInfo
     * through an instance of boost::typeindex::type_index.
     *
     * @note Either raw_service or unique_service must be set.
     */
    struct RegistrationInfo
    {
        RegistrationInfo()
            : raw_service(nullptr){};
        boost::typeindex::type_index service_interface;

        void *raw_service;
        std::unique_ptr<void, std::function<void(void *)>> unique_service;
    };
    using RegistrationInfoPtr = std::shared_ptr<RegistrationInfo>;

  public:
    using RegistrationHandle = std::weak_ptr<void>;

    /**
     * Register a service by passing an unique_ptr to it.
     *
     * By calling this overload over the more general
     * `register_service()`, the caller free themselves from managing
     * the lifetime of the service.
     *
     * The service will be automatically deleted once a successful call
     * to `unregister_service()` is made.
     */
    template <typename ServiceInterface>
    RegistrationHandle register_service(std::unique_ptr<ServiceInterface> srv)
    {
        auto type_index = boost::typeindex::type_id<ServiceInterface>();
        ASSERT_LOG(!services_.has(type_index),
                   "Already has a service registered for this interface: "
                       << type_index.pretty_name());

        RegistrationInfoPtr registration = std::make_shared<RegistrationInfo>();
        registration->service_interface  = type_index;
        registration->unique_service =
            std::unique_ptr<void, std::function<void(void *)>>(
                srv.release(), [](void *service_instance) {
                    auto typed_service_ptr =
                        static_cast<ServiceInterface *>(service_instance);
                    ASSERT_LOG(service_instance && typed_service_ptr,
                               "service_instance is null.");
                    delete typed_service_ptr;
                });
        services_.set(type_index, registration);

        return registration;
    }

    template <typename ServiceInterface>
    RegistrationHandle register_service(ServiceInterface *srv)
    {
        std::lock_guard<std::mutex> lg(mutex_);

        auto type_index = boost::typeindex::type_id<ServiceInterface>();
        ASSERT_LOG(!services_.has(type_index),
                   "Already has a service registered for this interface: "
                       << type_index.pretty_name());

        RegistrationInfoPtr registration = std::make_shared<RegistrationInfo>();
        registration->service_interface  = type_index;
        registration->raw_service        = srv;
        services_.set(type_index, registration);

        return registration;
    }

    /**
     * Unregister a service using the RegistrationHandle that was returned
     * from the `register_service()` call.
     *
     * @return This call returns true if the service was properly unregistered.
     * Otherwise it returns false.
     *
     * @note It's not possible to unregister a service if something is holding
     * a reference to it.
     */
    bool unregister_service(RegistrationHandle h)
    {
        std::lock_guard<std::mutex> lg(mutex_);

        std::shared_ptr<void> registration = h.lock();
        if (registration)
        {
            RegistrationInfoPtr registration_sptr =
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
            ASSERT_LOG(services_.has(registration_sptr->service_interface),
                       "Trying to unregister a service using an invalid handle.");
            services_.erase(registration_sptr->service_interface);
            return true;
        }
        return false;
    }

    /**
     * Unregister the service for the interface ServiceInterface.
     *
     * @return This call returns true if the service was properly unregistered.
     * Otherwise it returns false.
     *
     * @note It's not possible to unregister a service if something is holding
     * a reference to it.
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
        std::lock_guard<std::mutex> lg(mutex_);

        auto type_index = boost::typeindex::type_id<ServiceInterface>();
        if (services_.has(type_index))
        {
            auto registration = services_.get<RegistrationInfoPtr>(type_index);
            ServiceInterface *service_ptr;
            if (registration->raw_service)
                service_ptr =
                    static_cast<ServiceInterface *>(registration->raw_service);
            else
                service_ptr = static_cast<ServiceInterface *>(
                    registration->unique_service.get());
            return std::shared_ptr<ServiceInterface>(registration, service_ptr);
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
