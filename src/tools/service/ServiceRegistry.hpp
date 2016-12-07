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
#include "tools/bs2.hpp"
#include "tools/log.hpp"
#include "tools/registry/Registry.hpp"
#include <boost/type_index.hpp>

namespace Leosac
{

namespace service_event
{
enum EventType
{
    REGISTERED,
    UNREGISTERED,
};
class Event
{
  public:
    Event(EventType t)
        : type_(t)
    {
    }
    virtual ~Event() = default;
    EventType type() const
    {
        return type_;
    }

  private:
    EventType type_;
};

class ServiceRegistered : public Event
{
  public:
    ServiceRegistered(const boost::typeindex::type_index &interface_type)
        : Event(EventType::REGISTERED)
        , service_interface_type_(interface_type)
    {
    }
    boost::typeindex::type_index interface_type() const
    {
        return service_interface_type_;
    }

  private:
    boost::typeindex::type_index service_interface_type_;
};

class ServiceUnregistered : public Event
{
  public:
    ServiceUnregistered(const boost::typeindex::type_index &interface_type)
        : Event(EventType::UNREGISTERED)
        , service_interface_type_(interface_type)
    {
    }
    boost::typeindex::type_index interface_type() const
    {
        return service_interface_type_;
    }

  private:
    boost::typeindex::type_index service_interface_type_;
};
}

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
        RegistrationInfoPtr registration;
        {
            std::lock_guard<std::mutex> lg(mutex_);

            auto type_index = boost::typeindex::type_id<ServiceInterface>();
            ASSERT_LOG(!services_.count(type_index),
                       "Already has a service registered for this interface: "
                           << type_index.pretty_name());
            registration                    = std::make_shared<RegistrationInfo>();
            registration->service_interface = type_index;
            registration->unique_service =
                std::unique_ptr<void, std::function<void(void *)>>(
                    srv.release(), [](void *service_instance) {
                        auto typed_service_ptr =
                            static_cast<ServiceInterface *>(service_instance);
                        ASSERT_LOG(service_instance && typed_service_ptr,
                                   "service_instance is null.");
                        delete typed_service_ptr;
                    });
            services_.insert(std::make_pair(type_index, registration));
        }
        signal_registration(registration);
        return registration;
    }

    template <typename ServiceInterface>
    RegistrationHandle register_service(ServiceInterface *srv)
    {
        RegistrationInfoPtr registration;
        {
            std::lock_guard<std::mutex> lg(mutex_);

            auto type_index = boost::typeindex::type_id<ServiceInterface>();
            ASSERT_LOG(!services_.count(type_index),
                       "Already has a service registered for this interface: "
                           << type_index.pretty_name());

            registration                    = std::make_shared<RegistrationInfo>();
            registration->service_interface = type_index;
            registration->raw_service       = srv;
            services_.insert(std::make_pair(type_index, registration));
        }
        signal_registration(registration);
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
        bool success = false;
        RegistrationInfoPtr registration_sptr;
        {
            std::lock_guard<std::mutex> lg(mutex_);

            std::shared_ptr<void> registration = h.lock();
            if (registration)
            {
                registration_sptr =
                    std::static_pointer_cast<RegistrationInfo>(registration);

                // We must check that the service is not currently in use. If that's
                // the
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
                ASSERT_LOG(
                    services_.count(registration_sptr->service_interface),
                    "Trying to unregister a service using an invalid handle.");
                services_.erase(registration_sptr->service_interface);
                success = true;
            }
        }
        if (success)
            signal_deregistration(registration_sptr);
        return success;
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
        bool success = false;
        RegistrationInfoPtr registration;
        {
            std::lock_guard<std::mutex> lg(mutex_);
            boost::typeindex::type_index idx =
                boost::typeindex::type_id<ServiceInterface>();
            auto itr = services_.find(idx);
            if (itr != services_.end())
            {
                registration = itr->second;

                // Count: One in the registry, and the registration object and the
                // iterator
                if (registration.use_count() > 3)
                {
                    // Someone is using the service. Cannot unregister.
                    return false;
                }
                services_.erase(idx);
                success = true;
            }
        }
        if (success)
            signal_deregistration(registration);
        return success;
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
        auto itr        = services_.find(type_index);
        if (itr != services_.end())
        {
            auto &registration = itr->second;
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

    /**
     * Register a service-event listener.
     *
     * Service event listener must be both thread-safe and reentrant:
     *    + They may be invoked in any thread interacting with the registry.
     *    + A service event listener may interact with the registry, thus
     *      dispatching an other event synchronously.
     *
     * @note The internal registry lock is not hold while the event listener
     * are invoked.
     *
     * @warning You shall not keep hold of the ServiceEvent pointer that is passed
     * as a parameter to the listener. This pointer only valid while the listeners
     * are being invoked.
     */
    template <typename T>
    bs2::connection register_event_listener(T &&callable)
    {
        static_assert(std::is_convertible<T, EventListenerT>::value,
                      "Callable is not convertible to EventListenerT");
        return signal_.connect(callable);
    }

    using EventListenerT = std::function<void(const service_event::Event &)>;

  private:
    void signal_registration(const RegistrationInfoPtr &reg)
    {
        ASSERT_LOG(reg, "RegistrationInfoPtr is null");
        service_event::ServiceRegistered ev(reg->service_interface);
        signal_(ev);
    }

    void signal_deregistration(const RegistrationInfoPtr &reg)
    {
        ASSERT_LOG(reg, "RegistrationInfoPtr is null");
        service_event::ServiceUnregistered ev(reg->service_interface);
        signal_(ev);
    }

    mutable std::mutex mutex_;
    std::map<boost::typeindex::type_index, RegistrationInfoPtr> services_;
    bs2::signal<void(const service_event::Event &)> signal_;
};
}
