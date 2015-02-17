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
            namespace Strategy
            {
                class CardReading;

                using CardReadingUPtr = std::unique_ptr<CardReading>;

                /**
                * Interface for a strategy that read a card number.
                */
                class CardReading : public WiegandStrategy
                {
                public:
                    CardReading(WiegandReaderImpl *reader) :
                            WiegandStrategy(reader)
                    {
                    }

                    /**
                    * Returns the card id that was read.
                    */
                    virtual const std::string &get_card_id() const = 0;

                    /**
                    * Returns the number of bits in the card.
                    */
                    virtual int get_nb_bits() const = 0;
                };
            }
        }
    }
}
