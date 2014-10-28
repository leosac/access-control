#include <string>
#include <modules/sysfsgpio/SysFsGpioConfig.hpp>
#include <tools/log.hpp>
#include "gtest/gtest.h"

using namespace Leosac::Module::SysFsGpio;

extern thread_local zmqpp::socket *tl_log_socket;

/**
* Note this class doesnt inherits the TestHelper class.
* We only test the Config object.
*/
class SysFsGpioConfigTest : public ::testing::Test
{
public:
    // starts the led module
    SysFsGpioConfigTest()
    {
        ctx_= new zmqpp::context_t();
        tl_log_socket = new zmqpp::socket(*ctx_, zmqpp::socket_type::push);
        tl_log_socket->connect("inproc://trash");

        build_config_case1();
        build_config_case2();
        build_config_case3();
    }

    ~SysFsGpioConfigTest()
    {
        delete tl_log_socket;
        delete ctx_;
    }

    void build_config_case1()
    {
        boost::property_tree::ptree aliases_cfg, module_cfg;

        module_cfg.add("export_path", "/path/to/export");
        module_cfg.add("unexport_path", "/path/to/unexport");
        module_cfg.add("value_path", "/path/to/gpios/__REPLACE_ME__/value");
        module_cfg.add("edge_path", "/path/to/gpios/__REPLACE_ME__/edge");

        module_cfg.add_child("aliases", aliases_cfg);
        cfg_case_1_ = module_cfg;
    }

    void build_config_case2()
    {
        boost::property_tree::ptree aliases_cfg, module_cfg;

        module_cfg.add("export_path", "/path/to/export");
        module_cfg.add("unexport_path", "/path/to/unexport");
        module_cfg.add("value_path", "/path/to/gpios/__REPLACE_ME__/value");
        module_cfg.add("edge_path", "/path/to/gpios/__REPLACE_ME__/edge");

        // we define default aliases rules.
        aliases_cfg.add("default", "gpio__NO__");
        module_cfg.add_child("aliases", aliases_cfg);
        cfg_case_2_ = module_cfg;
    }

    void build_config_case3()
    {
        boost::property_tree::ptree aliases_cfg, module_cfg;

        module_cfg.add("export_path", "/path/to/export");
        module_cfg.add("unexport_path", "/path/to/unexport");
        module_cfg.add("value_path", "/path/to/gpios/__REPLACE_ME__/value");
        module_cfg.add("edge_path", "/path/to/gpios/__REPLACE_ME__/edge");

        // we define default aliases rules.
        aliases_cfg.add("default", "gpio__NO__");
        // lets say gpio with number 21 is somewhere else in the filesystem
        aliases_cfg.add("21", "gpio_magic_number");
        module_cfg.add_child("aliases", aliases_cfg);

        cfg_case_2_ = module_cfg;
    }

    zmqpp::context *ctx_;
    boost::property_tree::ptree cfg_case_1_;
    boost::property_tree::ptree cfg_case_2_;
};

TEST_F(SysFsGpioConfigTest, ExportUnexportPath)
{
    SysFsGpioConfig my_config(cfg_case_1_);

    ASSERT_EQ("/path/to/export", my_config.export_path());
    ASSERT_EQ("/path/to/unexport", my_config.unexport_path());
}

TEST_F(SysFsGpioConfigTest, PathAndDefaultAliases)
{
    SysFsGpioConfig my_config(cfg_case_2_);

    ASSERT_EQ("/path/to/gpios/gpio14/value", my_config.value_path(14));
    ASSERT_EQ("/path/to/gpios/gpio3/value", my_config.value_path(3));
    ASSERT_EQ("/path/to/gpios/gpio17/value", my_config.value_path(17));
}

TEST_F(SysFsGpioConfigTest, MoreComplexeAliases)
{
    SysFsGpioConfig my_config(cfg_case_2_);

    ASSERT_EQ("/path/to/gpios/gpio14/value", my_config.value_path(14));
    ASSERT_EQ("/path/to/gpios/gpio3/value", my_config.value_path(3));
    ASSERT_EQ("/path/to/gpios/gpio_magic_number/value", my_config.value_path(21));

    ASSERT_EQ("/path/to/gpios/gpio3/edge", my_config.edge_path(3));
    ASSERT_EQ("/path/to/gpios/gpio_magic_number/edge", my_config.edge_path(21));
}