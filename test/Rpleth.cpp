/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/Scheduler.hpp"
#include "core/auth/Auth.hpp"
#include "helper/FakeWiegandReader.hpp"
#include "helper/TestHelper.hpp"
#include "modules/rpleth/RplethModule.hpp"
#include "modules/rpleth/rplethprotocol.hpp"
#include "tools/log.hpp"
#include <memory>

using namespace Leosac::Module::Rpleth;
using namespace Leosac::Test::Helper;

namespace Leosac
{
namespace Test
{

class RplethTest : public Helper::TestHelper
{
  public:
    virtual bool run_module(zmqpp::socket *pipe)
    {
        boost::property_tree::ptree cfg, module_cfg;

        module_cfg.add("port", "4242");
        module_cfg.add("reader", "WIEGAND1");
        module_cfg.add("stream_mode", "true");

        cfg.add("name", "RPLETH");
        cfg.add_child("module_config", module_cfg);
        return test_run_module<RplethModule>(&ctx_, pipe, cfg);
    }

    RplethTest()
        : w1(ctx_, "WIEGAND1")
        , w1_actor(std::bind(&FakeWiegandReader::run, &w1, std::placeholders::_1))
    {
    }

    virtual ~RplethTest()
    {
    }

    RplethPacket extract_packet(const std::string &data)
    {
        ciruclar_buf_.write(reinterpret_cast<const unsigned char *>(data.c_str()),
                            data.size()); // write those to circular buffer
        RplethPacket rpleth_packet =
            RplethProtocol::decodeCommand(ciruclar_buf_, true);
        return rpleth_packet;
    }

    /**
    * Simulate client buffer.
    */
    CircularBuffer ciruclar_buf_;

    zmqpp::socket connect_to_rpleth()
    {
        zmqpp::message msg;
        std::string connection_identity, data;

        zmqpp::socket client(ctx_, zmqpp::socket_type::stream);
        client.connect("tcp://127.0.0.1:4242");
        client.receive(msg);

        assert(msg.parts() == 2);
        msg >> connection_identity;
        assert(msg.size(1) == 0);
        return std::move(client);
    }

    /**
    * Check that we can read a rpleth from the socket and check that its valid.
    */
    void check_rpleth_card_msg(zmqpp::socket &source,
                               const std::vector<uint8_t> card_binary)
    {
        zmqpp::message msg;
        std::string connection_identity, data;

        source.receive(msg);
        msg >> connection_identity;
        msg >> data; // data we would read from socket
        RplethPacket rpleth_packet = extract_packet(data);

        ASSERT_TRUE(rpleth_packet.isGood);
        ASSERT_EQ(8, +rpleth_packet.dataLen);
        ASSERT_EQ(8, +rpleth_packet.data.size());

        ASSERT_EQ(card_binary, rpleth_packet.data);
    }

    FakeWiegandReader w1;
    // to run the fake reader in a thread.
    zmqpp::actor w1_actor;
};

TEST(Rpleth, TestConvertCard)
{
    std::vector<uint8_t> out;
    std::vector<uint8_t> card_binary = {0x00, 0x00, 0x00, 0x00,
                                        0xff, 0xff, 0xff, 0xff};

    out = RplethModule::card_convert_from_text(std::make_pair("ff:ff:ff:ff", 32));
    ASSERT_EQ(card_binary, out);

    card_binary = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    out = RplethModule::card_convert_from_text(std::make_pair("00:00:00:00", 32));
    ASSERT_EQ(card_binary, out);

    card_binary = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x05, 0x85};
    out = RplethModule::card_convert_from_text(std::make_pair("80:81:61:40", 26));
    ASSERT_EQ(card_binary, out);

    card_binary = {0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x61, 0x40};
    out = RplethModule::card_convert_from_text(std::make_pair("80:81:61:40", 32));
    ASSERT_EQ(card_binary, out);
}

/**
* Check that a rpleth receive cards that are read.
*/
TEST_F(RplethTest, TestReceiveStreamCardsSimple)
{
    zmqpp::socket client = connect_to_rpleth();

    // fake wiegand reader activity.
    bus_push_.send(zmqpp::message() << "S_WIEGAND1"
                                    << Leosac::Auth::SourceType::SIMPLE_WIEGAND
                                    << "ff:ab:cd:ef" << 32);
    check_rpleth_card_msg(client, {0x00, 0x00, 0x00, 0x00, 0xff, 0xab, 0xcd, 0xef});
}

TEST_F(RplethTest, TestReceiveStreamCards2)
{
    zmqpp::message msg;
    std::string connection_identity, data;
    zmqpp::socket client = connect_to_rpleth();

    // fake wiegand reader activity.
    bus_push_.send(zmqpp::message() << "S_WIEGAND1"
                                    << Leosac::Auth::SourceType::SIMPLE_WIEGAND
                                    << "ff:ab:cd:ef" << 32);
    check_rpleth_card_msg(client, {0x00, 0x00, 0x00, 0x00, 0xff, 0xab, 0xcd, 0xef});

    bus_push_.send(zmqpp::message() << "S_WIEGAND1"
                                    << Leosac::Auth::SourceType::SIMPLE_WIEGAND
                                    << "11:22:33:44" << 32);
    check_rpleth_card_msg(client, {0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44});

    bus_push_.send(zmqpp::message() << "S_WIEGAND1"
                                    << Leosac::Auth::SourceType::SIMPLE_WIEGAND
                                    << "80:81:61:40" << 26);
    check_rpleth_card_msg(client, {0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x05, 0x85});

    bus_push_.send(zmqpp::message() << "S_IGNORED_READER"
                                    << Leosac::Auth::SourceType::SIMPLE_WIEGAND
                                    << "11:22:33:44" << 32);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // nothing to read anymore
    ASSERT_FALSE(client.receive(msg, true));
}
}
}
