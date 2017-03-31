/*
    Copyright (C) 2014-2016 Leosac

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

#include "PushSimpleCardNumber.hpp"
#include "core/credentials/RFIDCard.hpp"

// fixme zmqpp include problem
#include <cstring>

#include <zmqpp/inet.hpp>
#include <zmqpp/zmqpp.hpp>

Leosac::ByteVector Leosac::Module::TCPNotifier::PushSimpleCardNumber::build_cred_msg(
    const Cred::RFIDCard &card)
{
    ByteVector data(8);
    uint64_t network_card_id = zmqpp::htonll(card.to_int());
    std::memcpy(&data[0], &network_card_id, 8);

    return data;
}
