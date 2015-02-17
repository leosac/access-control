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
            class SimpleWiegandStrategy;
            using SimpleWiegandStrategyUPtr = std::unique_ptr<SimpleWiegandStrategy>;

            /**
            * Implementation of a wiegand card only strategy.
            */
            class SimpleWiegandStrategy : public WiegandStrategy
            {
            public:
                SimpleWiegandStrategy(WiegandReaderImpl *reader);

                virtual void timeout() override;

                virtual bool completed() const override;

                virtual void signal() override;

                /**
                * Returns the card id that was read.
                */
                const std::string &get_card_id() const;

                /**
                * Returns the number of bits in the card.
                */
                int get_nb_bits() const;

            private:
                bool ready_;
                int nb_bits_;
                std::string card_id_;
            };
        }
    }
}
