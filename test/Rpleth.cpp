#include <memory>
#include <modules/rpleth/RplethModule.hpp>
#include <modules/rpleth/rplethprotocol.hpp>
#include "helper/TestHelper.hpp"
#include "helper/FakeWiegandReader.hpp"

class RplethTest : public TestHelper
{
public:
    virtual bool run_module(zmqpp::socket *pipe)
    {
        boost::property_tree::ptree cfg, module_cfg;

        module_cfg.add("port", "4242");
        module_cfg.add("reader", "WIEGAND1");
        module_cfg.add("stream_mode", "true");

        cfg.add_child("module_config", module_cfg);
        return test_run_module<RplethModule>(&ctx_, pipe, cfg);
        module_ = new RplethModule(ctx_, pipe, cfg);


        pipe->send(zmqpp::signal::ok);
        module_->run();

        return true;
    }

    RplethTest() : module_(nullptr),
    w1(ctx_, "WIEGAND1"),
    w1_actor(std::bind(&FakeWiegandReader::run, &w1, std::placeholders::_1))
    {}

    virtual ~RplethTest()
    {
        delete module_;
    }

    RplethModule *module_;

    FakeWiegandReader w1;
    // to run the fake reader in a thread.
    zmqpp::actor w1_actor;
};

TEST_F(RplethTest, TestConvertCard)
{
/*    std::vector<uint8_t> card_binary = {0xff, 0xff, 0xff, 0xff};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("ff:ff:ff:ff"));

    card_binary = {0x32, 0x12, 0x14, 0xae, 0xbc};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("32:12:14:ae:bc"));

    card_binary = {0x00, 0x00, 0x00, 0x00,};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("00:00:00:00"));*/
}

// listen for input cards
TEST_F(RplethTest, TestListenToCards)
{
    zmqpp::message msg, msg2;
    std::string connection_identity, data;

    zmqpp::socket client(ctx_, zmqpp::socket_type::stream);
    client.connect("tcp://127.0.0.1:4242");

    client.receive(msg);
    ASSERT_EQ(2, msg.parts());
    msg >> connection_identity;
    ASSERT_EQ(0, msg.size(1));

    // fake wiegand reader activity.
    bus_push_.send(zmqpp::message() << "S_WIEGAND1" << "ff:ab:cd:ef:12");

    client.receive(msg2);
    CircularBuffer circular_buf;
    msg2 >> connection_identity;
    msg2 >> data; // data we would read from socket
    std::cout << "size =  " << (int) data.size() << std::endl;
    circular_buf.write(reinterpret_cast<const unsigned char *>(data.c_str()), data.size()); // write those to circular buffer
    RplethPacket rpleth_packet = RplethProtocol::decodeCommand(circular_buf, true);

    ASSERT_TRUE(rpleth_packet.isGood);
    ASSERT_EQ(5, rpleth_packet.dataLen);
    ASSERT_EQ(5, rpleth_packet.data.size());
    std::vector<uint8_t> binary_card = {0xff, 0xab, 0xcd, 0xef, 0x12};
    ASSERT_EQ(binary_card, rpleth_packet.data);
}
