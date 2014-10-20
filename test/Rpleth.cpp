#include <modules/rpleth/RplethModule.hpp>
#include "helper/TestHelper.hpp"

class RplethTest : public TestHelper
{
public:
    virtual bool run_module(zmqpp::socket *pipe)
    {
        boost::property_tree::ptree cfg, module_cfg;

        module_cfg.add("port", "4242");
        module_cfg.add("reader", "WIEGAND1");
        module_cfg.add("stream_mode", "ON");

        cfg.add_child("module_config", module_cfg);
        module_ = new RplethModule(ctx_, pipe, cfg);
        pipe->send(zmqpp::signal::ok);
        return true;
    }

    virtual ~RplethTest()
    {
        delete module_;
    }

    RplethModule *module_;
};

TEST_F(RplethTest, TestConvertCard)
{
    std::vector<uint8_t> card_binary = {0xff, 0xff, 0xff, 0xff};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("ff:ff:ff:ff"));

    card_binary = {0x32, 0x12, 0x14, 0xae, 0xbc};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("32:12:14:ae:bc"));

    card_binary = {0x00, 0x00, 0x00, 0x00,};
    ASSERT_EQ(card_binary, module_->card_convert_from_text("00:00:00:00"));
}
