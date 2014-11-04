#pragma once

#include "core/auth/ICard.hpp"
#include <string>

namespace Leosac
{
    namespace Auth
    {
        class IAccessProfile;
        class IUser;
        /**
        * A wiegand card.
        */
        class WiegandCard : public ICard
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
            * Number of meaningfull bit
            */
            int nb_bits_;
        };
    }
}
