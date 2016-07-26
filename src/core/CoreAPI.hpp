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

#include <cstdint>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace Leosac
{

class Kernel;

/**
 * CoreAPI is a class for interact with Leosac's core.
 *
 * This class is intended to be used by modules, as a way to
 * retrieve information from Leosac's Kernel in a clean way.
 *
 * This class uses message passing / tasks scheduling internally, and
 * expose an easy to use API to client code.
 *
 * @note This object itself is NOT thread safe. However it provides a
 * safe way to interact with the core.
 *
 * @warning You SHALL NOT use this class from the main thread.
 */
class CoreAPI
{
      public:
        explicit CoreAPI(Kernel &k);

        /**
         * Retrieve the local configuration version.
         */
        uint64_t config_version() const;

        /**
         * Retrieve the property tree describing the Leosac's kernel
         * configuration.
         */
        boost::property_tree::ptree kernel_config() const;

        /**
         * Returns the `instance_name` of Leosac.
         */
        std::string instance_name() const;

        /**
         * Returns the uptime, in seconds.
         */
        uint64_t uptime() const;

        /**
         * Retrieve the names of all enabled modules.
         */
        std::vector<std::string> modules_names() const;

      private:
        Kernel &kernel_;
};
}
