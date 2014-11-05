#pragma once

#include "core/auth/BaseAuthSource.hpp"
#include <string>

namespace Leosac
{
    namespace Auth
    {
        class WiegandCard;
        using WiegandCardPtr = std::shared_ptr<WiegandCard>;
        /**
        * A wiegand card.
        */
        class WiegandCard : public BaseAuthSource
        {
        public:
            /**
            * Create a WiegandCard object.
            *
            * @param card_id the id of the card in hexadecimal text format
            * @param bits number of bits (wiegand 26, wiegand32 ...)
            */
            WiegandCard(const std::string &card_id, int bits);

        protected:
            /**
            * Card id
            */
            std::string id_;

            /**
            * Number of meaningful bit
            */
            int nb_bits_;
        };
    }
}
