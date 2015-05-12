/*
    Copyright (C) 2014-2015 Islog

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

#include <map>
#include <mutex>

namespace Leosac
{
    /**
     * This class is here to help check the validity of the configuration.
     *
     * Leosac configuration being text-based, various object (LED, Reader, GPIO)
     * are referenced by name. This is practical because it gives the configuration
     * maintainer some flexibility (especially regarding the Doorman module).
     * However, it is also difficult to detect misconfiguration before running
     * into trouble at runtime.
     *
     * How does this class help?
     *
     * The idea is for module declaring object to register them with this class, and
     * for module using (aka talking-to) objects, to check here first.
     *
     * As a result, this class shall be fully thread safe.
     *
     * @note The current behavior is to assert when registering an object twice.
     */
    class ConfigChecker
    {
    public:
        enum class ObjectType
        {
            GPIO        = 0,
            LED         = 1,
            READER      = 2,
            BUZZER      = 3
        };

        /**
         * Declare an object on the registry.
         */
        void register_object(const std::string &name, const ObjectType& type);

        /**
         * Check whether or not an object with name `name` is declared
         * in the registry, no matter its type.
         */
        bool has_object(const std::string &name) const;

        /**
         * Check whether or not an object with name `name` is declared
         * in the registry and if check that its type is `type`.
         */
        bool has_object(const std::string &name, const ObjectType &type) const;

        /**
         * Clear all informations.
         * This is useful when synchronizing with remote.
         */
        void clear();

    private:
        std::map<std::string, ObjectType> objects_;
        mutable std::mutex mutex_;
    };
}
