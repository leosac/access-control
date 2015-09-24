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

#include "gtest/gtest.h"
#include "core/auth/WiegandCard.hpp"
#include "modules/tcp-notifier/protocols/Megasoft.hpp"

using namespace Leosac::Auth;

namespace Leosac
{
    namespace Test
    {
        TEST(TestTcpNotifierProtocol, Megasoft0)
        {
            Module::TCPNotifier::MegasoftProtocol proto;
            WiegandCard c1("ff:ff:ff:ff", 32);

            auto protocol_bytes = proto.build_cred_msg(c1);
            auto protocol_str = std::string(protocol_bytes.begin(), protocol_bytes.end());
            ASSERT_EQ("check_card: data: <'4294967295\\r'> <<type 'str'>>",
                      protocol_str);
        }

        TEST(TestTcpNotifierProtocol, Megasoft1)
        {
            Module::TCPNotifier::MegasoftProtocol proto;
            WiegandCard c1("00:00:00:00", 32);

            auto protocol_bytes = proto.build_cred_msg(c1);
            auto protocol_str = std::string(protocol_bytes.begin(), protocol_bytes.end());
            ASSERT_EQ("check_card: data: <'0\\r'> <<type 'str'>>",
                      protocol_str);
        }

        TEST(TestTcpNotifierProtocol, Megasoft3)
        {
            Module::TCPNotifier::MegasoftProtocol proto;
            WiegandCard c1("53:5E:4D:4F", 32);

            auto protocol_bytes = proto.build_cred_msg(c1);
            auto protocol_str = std::string(protocol_bytes.begin(), protocol_bytes.end());
            ASSERT_EQ("check_card: data: <'1330470483\\r'> <<type 'str'>>",
                      protocol_str);
        }
    }
}
