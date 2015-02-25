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
            * Default credential is enabled and no time-base limitation.
            */
            CredentialValidity();

            CredentialValidity(const CredentialValidity &) = default;

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

            /**
            * Set the start validity date.
            * We receive the date as a string. The expected format is this: `%d/%m/%Y %H:%M`
            *
             This method accept an empty string, if that case it will set the end_date to `time_point::min()`.
            */
            void set_start_date(const std::string &s);

            /**
            * Set the end validity date.
            * We receive the date as a string. The expected format is this: `%d/%m/%Y %H:%M`
            *
            * This method accept an empty string, if that case it will set the end_date to `time_point::max()`.
            */
            void set_end_date(const std::string &s);

            void set_enabled(bool v);

        private:
            using TimePoint = std::chrono::system_clock::time_point;

            TimePoint validity_start_;
            TimePoint validity_end_;
            bool enabled_;
        };
    }
}
