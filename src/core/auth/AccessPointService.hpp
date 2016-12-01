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

#include <boost/type_index.hpp>
#include <map>
#include <tools/log.hpp>
#include <type_traits>

namespace Leosac
{
namespace Auth
{

struct AccessPointBackend
{
};

/**
 * This service lets various AccessPoint backend register
 * and provide implementation to use by the AccessPointCRUD object.
 *
 *  The registration is done by specifying the name (string) of
 *  the controller module for the type of AccessPoint.
 */
class AccessPointService
{
    void register_backend(const std::string &controller_module,
                          AccessPointBackend *backend)
    {
        ASSERT_LOG(backends_.count(controller_module) == 0,
                   "A backend for this serializer is already registered.");
        backends_[controller_module] = backend;
    }

    /**
     * Get the AccessPointBackend that supports the given
     * `controller_module`.
     * @return nullptr if none found.
     */
    AccessPointBackend *get_backend(const std::string &controller_module);

    /**
     * The various backend for various type of AccessPoint.
     * Those are registered by modules.
     */
    std::map<std::string, AccessPointBackend *> backends_;
};
}
}
