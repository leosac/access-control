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

#include "WiegandStrategy.hpp"

namespace Leosac
{
    namespace Module
    {

        namespace Wiegand
        {
            /**
            * Strategy for PIN only, when up to 5 keys are buffered and sent
            * as 26 bits. See HID documentation about keypad buffered mode.
            */
            class WiegandPinBuffered : public WiegandStrategy
            {
            public:
                /**
                * Create a strategy that read 4bits-per-key PIN code.
                *
                * @param reader the reader object we provide the strategy for.
                */
                WiegandPinBuffered(WiegandReaderImpl *reader);

                virtual void timeout() override;

                virtual bool completed() const override;

                virtual void signal() override;

                const std::string &get_pin() const;

                virtual void reset() override;

            private:
                bool ready_;
                std::string pin_;
            };
        }
    }
}
