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

#include "tools/log.hpp"
#include "tools/registry/Registry.hpp"
#include <boost/any.hpp>
#include <map>

namespace Leosac
{
/**
 * A simple, thread-local container to share various
 * type of objects.
 *
 * @note It uses an internal enumeration as a key type.
 */
class ThreadLocalRegistry
{
  public:
    enum KeyType
    {
        DATABASE
    };

    using UnderlyingRegistry = Registry<KeyType>;

    template <typename T>
    static T get(const KeyType &key)
    {
        return registry_.get<T>(key);
    }

    template <typename T>
    static void set(const KeyType &key, const T &obj)
    {
        registry_.set(key, obj);
    }

    template <typename T>
    static void set(const KeyType &key, const T &obj,
                    const UnderlyingRegistry::Clock::time_point &expire_at)
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

    static void purge()
    {
        return registry_.purge();
    }

  private:
    static thread_local UnderlyingRegistry registry_;
};
}
