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

#include "log.hpp"
#include <boost/any.hpp>
#include <map>

namespace Leosac
{
/**
 * A simple, thread-local, global container to share object.
 *
 * This is intended to be mostly use while debugging.
 */
class GlobalRegistry
{
  private:
    template <typename T>
    static bool is_type_valid(const boost::any &a)
    {
        try
        {
            boost::any_cast<T>(a);
            return true;
        }
        catch (const boost::bad_any_cast &)
        {
            return false;
        }
    }

  public:
    // Keys
    enum Keys
    {
        DATABASE
    };

    using KeyType = enum Keys;

    template <typename T>
    static T get(const KeyType &key)
    {
        auto itr = store_.find(key);
        ASSERT_LOG(itr != store_.end(), "Cannot find key in registry.");

        ASSERT_LOG(is_type_valid<T>(itr->second),
                   "The object type for key " << key
                                              << " mismatch the required type.");
        return boost::any_cast<T>(itr->second);
    }

    template <typename T>
    static void set(const KeyType &key, const T &obj)
    {
        store_[key] = boost::any(obj);
    }

  private:
    static thread_local std::map<KeyType, boost::any> store_;
};
}