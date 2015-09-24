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

#include "modules/tcp-notifier/ProtocolHandler.hpp"

namespace Leosac
{
  namespace Module
  {
    namespace TCPNotifier
    {
      class MegasoftProtocol : public ProtocolHandler
      {
      public:
        virtual ByteVector build_cred_msg(const Auth::WiegandCard &card) override;

      private:
        /**
         * Build a protocol frame ready to send over the network.
         */
        ByteVector build_protocol_frame(uint32_t card_id);


        /**
         * Perform some check on the card representation, making sure it is
         * suitable for use by this protocol.
         *
         * If validation fails, this function will throw ProtocolException.
         */
        void validate(const Auth::WiegandCard &card);
      };
    }
  }
}