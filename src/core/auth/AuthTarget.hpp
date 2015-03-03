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

#include <memory>
#include <string>
#include <hardware/FGPIO.hpp>
#include <tools/Schedule.hpp>

namespace Leosac
{
    namespace Auth
    {
        class AuthTarget;
        using AuthTargetPtr = std::shared_ptr<AuthTarget>;

        /**
        * Represent an object that we are authorizing against (a door).
        */
        class AuthTarget
        {
        public:
            virtual ~AuthTarget() = default;
            explicit AuthTarget(const std::string target_name);

            const std::string &name() const;
            void name(const std::string &new_name);

            void add_always_open_sched(const Tools::Schedule &sched);
            void add_always_close_sched(const Tools::Schedule &sched);

            /**
            * Check whether the door is in "always open" mode at the given time point.
            */
            bool is_always_open(const std::chrono::system_clock::time_point &tp) const;

            /**
            * Check whether the door is in "always closed" mode at the given time point.
            */
            bool is_always_closed(const std::chrono::system_clock::time_point &tp) const;

            /**
            * Returns the pointer to the optional FGPIO associated with the door.
            * It may be NULL.
            *
            * Do not free or keep reference on it longer than the lifetime of the AuthTarget object.
            * If you ever set a new gpio() pointer for this AuthTarget, the previous reference will
            * become invalid.
            */
            Hardware::FGPIO *gpio();

            void gpio(std::unique_ptr<Hardware::FGPIO> new_gpio);

        protected:
            std::string name_;

            std::vector<Tools::Schedule> always_open_;
            std::vector<Tools::Schedule> always_close_;

            /**
            * Optional GPIO associated with the door.
            */
            std::unique_ptr<Hardware::FGPIO> gpio_;
        };
    }
}