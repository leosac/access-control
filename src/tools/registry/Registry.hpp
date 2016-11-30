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

#include "exception/leosacexception.hpp"
#include "tools/log.hpp"
#include <boost/any.hpp>
#include <map>

namespace Leosac
{

class RegistryKeyNotFoundException : public LEOSACException
{
  public:
    RegistryKeyNotFoundException(const std::string &key)
        : LEOSACException(BUILD_STR("Cannot find key " << key << " in registry."))
    {
    }
};

/**
 * A simple container that can holds object of any type.
 *
 * This is basically a map<string, boost::any>. Type safety
 * is enforced at runtime, and trying to retrieve an object of type
 * that doesn't match will result in an assertion.
 *
 *
 * While the registry supports concept of expiration for its entry,
 * it is not designed to prevent access to expired object. Instead, its
 * goal is to remove expired from time to time to prevent infinite memory
 * grows.
 *
 * @note The registry is templated on the key type.
 */
template <typename KeyType = std::string>
class Registry
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
    using Clock = std::chrono::steady_clock;
    /**
     * Specialization for key of type std::string.
     */
    template <typename T>
    std::enable_if_t<std::is_same<KeyType, std::string>::value &&
                         !std::is_same<KeyType, const char *>::value,
                     T>
    get(const std::string &key) const
    {
        auto itr = store_.find(key);
        if (itr == store_.end())
            throw RegistryKeyNotFoundException(key);

        ASSERT_LOG(is_type_valid<T>(itr->second),
                   "The object type for key " << key
                                              << " mismatch the required type.");
        return boost::any_cast<T>(itr->second);
    }

    /**
     * Specialization for string literals.
     */
    template <typename T>
    std::enable_if_t<std::is_same<KeyType, const char *>::value &&
                         !std::is_same<KeyType, std::string>::value,
                     T>
    get(const char *key) const
    {
        return get<T>(std::string(key));
    }

    template <typename T>
    std::enable_if_t<!std::is_same<KeyType, const char *>::value &&
                         !std::is_same<KeyType, std::string>::value,
                     T>
    get(const KeyType &key) const
    {
        auto itr = store_.find(key);
        if (itr == store_.end())
            throw RegistryKeyNotFoundException(
                "NOT_SPECIFIED_BECAUSE_IS_NOT_STRING");

        ASSERT_LOG(is_type_valid<T>(itr->second),
                   "The object type for key " << key
                                              << " mismatch the required type.");
        return boost::any_cast<T>(itr->second);
    }

    template <typename T>
    void set(const KeyType &key, const T &obj)
    {
        purge();
        store_[key] = boost::any(obj);
    }

    template <typename T>
    void set(const KeyType &key, const T &obj,
             const std::chrono::steady_clock::time_point &expire_at)
    {
        purge();
        store_[key]  = boost::any(obj);
        expiry_[key] = expire_at;
    }

    /**
     * Erase an entry for the store.
     *
     * If the store has no such entry, does nothing.
     */
    void erase(const KeyType &key)
    {
        store_.erase(key);
        expiry_.erase(key);
    }

    bool has(const KeyType &key) const
    {
        return store_.find(key) != store_.end();
    }

    /**
     * Remove expired entry from the store.
     */
    void purge()
    {
        std::vector<KeyType> to_remove;

        auto now = Clock::now();
        for (const auto &expiry_info : expiry_)
        {
            if (now > expiry_info.second)
            {
                to_remove.push_back(expiry_info.first);
            }
        }

        for (const auto &key : to_remove)
        {
            erase(key);
        }
    }

  private:
    std::map<KeyType, Clock::time_point> expiry_;
    std::map<KeyType, boost::any> store_;
};
}
