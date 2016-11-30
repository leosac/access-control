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

#include "tools/log.hpp"
#include "tools/registry/Registry.hpp"
#include <boost/any.hpp>
#include <map>
#include <mutex>

namespace Leosac
{
/**
 * An application wide registry that can store objects of any type.
 */
class GlobalRegistry
{
  public:
    using KeyType            = std::string;
    using UnderlyingRegistry = Registry<KeyType>;
    using Clock              = UnderlyingRegistry::Clock;

    template <typename T>
    static T get(const KeyType &key)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        return registry_.get<T>(key);
    }

    template <typename T>
    static void set(const KeyType &key, const T &obj)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        registry_.set(key, obj);
    }

    template <typename T>
    static void set(const KeyType &key, const T &obj,
                    const std::chrono::steady_clock::time_point &expire_at)
    {
        registry_.set(key, obj, expire_at);
    }

    static void erase(const KeyType &key)
    {
        return registry_.erase(key);
    }

    static bool has(const KeyType &key)
    {
        return registry_.has(key);
    }

  private:
    static std::mutex mutex_;
    static UnderlyingRegistry registry_;
};
}
