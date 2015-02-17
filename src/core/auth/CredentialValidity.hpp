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

#include <chrono>
#include <string>

namespace Leosac
{
    namespace Auth
    {
        /**
        * A simple class that stores (and can be queried for) the validity of some credentials.
        * Validity means 3 simple thing:
        *      + Is is enabled ?
        *      + The validity start date.
        *      + The validity end date.
        *
        * You can set validity_start to time::min(), and validity_end to time::max
        * if there is no validity date.
        */
        class CredentialValidity
        {
        public:
            /**
            * Check that the current date is between
            * validity start and end and make sure its
            * enabled too.
            */
            bool is_valid() const;

            /**
            * Is the credential enabled ?
            */
            bool is_enabled() const;

            /**
            * Are we currently is the validity range of the credential ?
            */
            bool is_in_range() const;

        private:
            using TimePoint = std::chrono::system_clock::time_point;

            TimePoint validity_start;
            TimePoint validity_end;
            bool enabled_;
        };
    }
}
