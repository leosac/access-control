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

#include "tools/AssertCast.hpp"
#include <boost/type_index.hpp>
#include <map>
#include <mutex>
#include <type_traits>

namespace Leosac
{
/**
 * Provides dynamic serializers management for a given object's
 * hierarchy.
 *
 * Clients are able to add serializers for a concrete subtype of
 * ObjectT. They can also remove serializers, and call the
 * `serialize()` method.
 *
 * @note: The ExtensibleSerializer is thread safe. Management of user-defined
 * serializers, aswell as invocation of serializers is performed while
 * holding an internal recursive mutex. It is therefore allowed for a
 * serializer implementation to make call to the ExtensibleSerializer.
 * However, be careful to avoid deadlock if using multiple ExtensibleSerializer.
 *
 *
 * @tparam SerializedT The type of serialization output.
 * @tparam ObjectT The base type of the class hierarchy this serializer
 * will be able to handle.
 * @tparam AdditionalArgs A variadic parameter that let the `serialize()`
 * method accepts additional arguments and forward them
 * to the user-provided serialization callable.
 */
template <typename SerializedT, typename ObjectT, typename... AdditionalArgs>
class ExtensibleSerializer
{
  public:
    SerializedT serialize(const ObjectT &input, AdditionalArgs &&... args) const
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);

        auto type_index = boost::typeindex::type_id_runtime(input);
        auto itr        = serializers_.find(type_index);
        if (itr != serializers_.end())
        {
            // Invoke the adapter we stored in the map.
            // The wrapper will invoke the user-defined callable.
            return itr->second(input, std::forward<AdditionalArgs>(args)...);
        }
        ASSERT_LOG(false, "Cannot find an appropriate serializer for " +
                              type_index.pretty_name());
        return {};
    }

    /**
     * Callable type for serialization.
     */
    using SerializationCallable =
        std::function<SerializedT(const ObjectT &, AdditionalArgs &&...)>;

    /**
     * Register a serializer for T.
     */
    template <typename T, typename Callable>
    void register_serializer(Callable &&callable)
    {
        static_assert(std::is_base_of<ObjectT, T>::value,
                      "T is not a subclass of ObjectT");

        std::lock_guard<decltype(mutex_)> lg(mutex_);
        auto type_index = boost::typeindex::type_id<T>();
        ASSERT_LOG(serializers_.count(type_index) == 0,
                   "Already got a serializer for this type of object.");

        // This is an adapter that wraps the user function. This allows us to
        // store uniform function objects.
        auto wrapper = [callable](const ObjectT &base_obj,
                                  AdditionalArgs &&... args) -> SerializedT {
            // This cast is safe, because we'll only pick this serializer when
            // the typeid of the base type (ObjectT) match T.
            const T &concrete_object = reinterpret_cast<const T &>(base_obj);

            // Extra runtime safety check.
            const T &extra_safety = assert_cast<const T &>(base_obj);
            ASSERT_LOG(std::addressof(concrete_object) ==
                           std::addressof(extra_safety),
                       "Memory error.");
            return callable(concrete_object, std::forward<AdditionalArgs>(args)...);
        };
        serializers_[type_index] = wrapper;
    };

    template <typename T>
    void unregister_serializer()
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);

        auto type_index = boost::typeindex::type_id<T>();
        serializers_.erase(type_index);
    }

  private:
    mutable std::recursive_mutex mutex_;
    std::map<boost::typeindex::type_index, SerializationCallable> serializers_;
};
}
