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

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <tools/log.hpp>
#include "Megasoft.hpp"
#include "core/auth/WiegandCard.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::TCPNotifier;

void MegasoftProtocol::validate(const Auth::WiegandCard &card)
{
  if (card.card_id().size() != 11) // "AA:BB:CC:DD"
  {
    throw ProtocolException("Invalid card id: " + card.card_id());
  }
  if (card.nb_bits() != 32)
  {
    throw ProtocolException("Card doesn't have 32 meaningful bits");
  }
}

ByteVector MegasoftProtocol::build_cred_msg(const Auth::WiegandCard &card)
{
  // According to the protocol, the UID is 4bytes.
  // We have to take each byte - in reverse order -
  // and build the csn from this.

  // The CSN is then converted to a decimal string
  // and inserted in the protocol message.

  validate(card);
  auto csn_hex = card.card_id();

  uint32_t card_id = 0;
  std::stringstream ss(csn_hex);
  for (int i = 0; i < 4; ++i)
  {
    uint16_t tmp = 0;
    ss >> std::hex >> tmp;

    if (tmp > 255)
    {
      throw ProtocolException(
          "CardId (" + csn_hex +
          ") is wrong. We aren't supposed to read more than 1 byte.");
    }

    card_id |= tmp << (i * 8);
    if (i != 3)
    {
      uint8_t trash = 0;
      ss >> trash;
      ASSERT_LOG(trash == ':', "Invalid separator (" << trash << ")");
    }
  }
  return build_protocol_frame(card_id);
}

ByteVector MegasoftProtocol::build_protocol_frame(uint32_t card_id)
{
  std::string card_id_txt = std::to_string(card_id);
  std::string protocol_frame =
      "check_card: data: <'CARD_ID_PLACEHOLDER\\r'> <<type 'str'>>";
  auto final_str = boost::replace_all_copy(protocol_frame,
                                           "CARD_ID_PLACEHOLDER", card_id_txt);
  ByteVector ret(final_str.begin(), final_str.end());
  return ret;
}
