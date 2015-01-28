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

#include <string>
#include "core/config/ConfigManager.hpp"
#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include "tools/XmlPropertyTree.hpp"

/**
* Path to test-data file.
* this come from command line (see CMakeLists.txt)
*/
static std::string gl_data_path;

namespace Leosac
{
    namespace Test
    {
        class ConfigManagerTest : public ::testing::Test
        {
        public:
            ConfigManagerTest()
            {
                cfg0 = std::make_shared<ConfigManager>(boost::property_tree::ptree());

                auto cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path + "ConfigManager1.xml");
                cfg1 = std::make_shared<ConfigManager>(cfg_tree.get_child("kernel"));

                cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path + "ConfigManager2.xml");
                cfg2 = std::make_shared<ConfigManager>(cfg_tree.get_child("kernel"));

                cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path + "ConfigManager3.xml");
                cfg3 = std::make_shared<ConfigManager>(cfg_tree.get_child("kernel"));
            }

        protected:
            std::shared_ptr<ConfigManager> cfg0;
            std::shared_ptr<ConfigManager> cfg1;
            std::shared_ptr<ConfigManager> cfg2;
            std::shared_ptr<ConfigManager> cfg3;
        };


        TEST_F(ConfigManagerTest, store_load)
        {
            boost::property_tree::ptree my_module_cfg;

            ASSERT_FALSE(cfg0->store_config("my", my_module_cfg));
            // no we overwrite, so should return true.
            ASSERT_TRUE(cfg0->store_config("my", my_module_cfg));

            auto cfg = cfg0->load_config("my");
            ASSERT_EQ(my_module_cfg, cfg);
        }

        TEST_F(ConfigManagerTest, access_cfg)
        {
            auto network_cfg = cfg1->kconfig().get_child("network");

            bool enabled = network_cfg.get<bool>("enabled");
            ASSERT_EQ(false, enabled);
        }

        TEST_F(ConfigManagerTest, build_cfg)
        {
            auto cfg = cfg1->get_general_config();

            auto c1 = cfg.get_child("plugin_directories");
            auto c2 = cfg.get_child("log");
            auto c3 = cfg.get_child("network");
        }

        /**
        * Like build_cfg test, but we build a config tree for export.
        * We must make sure that information from `<sync_source>` are honored.
        */
        TEST_F(ConfigManagerTest, build_safe_cfg)
        {
            auto cfg = cfg1->get_exportable_general_config();

            auto c1 = cfg.get_child_optional("plugin_directories");
            auto c2 = cfg.get_child_optional("log");
            auto c3 = cfg.get_child_optional("network");

            ASSERT_FALSE(c1);
            ASSERT_TRUE(c2);
            ASSERT_FALSE(c3);
        }

        /**
        * Safe as build_safe_cfg but with a second config file.
        * We do not declare <sync_source> there, so we should retrieve
        * everything.
        */
        TEST_F(ConfigManagerTest, build_safe_cfg2)
        {
            auto cfg = cfg2->get_exportable_general_config();

            auto c1 = cfg.get_child_optional("plugin_directories");
            auto c2 = cfg.get_child_optional("log");
            auto c3 = cfg.get_child_optional("network");

            ASSERT_TRUE(c1);
            ASSERT_TRUE(c2);
            ASSERT_TRUE(c3);
        }

        TEST_F(ConfigManagerTest, build_safe_cfg3)
        {
            auto cfg = cfg3->get_exportable_general_config();

            auto c1 = cfg.get_child_optional("plugin_directories");
            auto c2 = cfg.get_child_optional("log");
            auto c3 = cfg.get_child_optional("network");

            ASSERT_FALSE(c1);
            ASSERT_FALSE(c2);
            ASSERT_FALSE(c3);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // gtest shall leave us with our arguments.
    // argv[1] shall be the path to test data file
    assert(argc == 2);
    gl_data_path = std::string(argv[1]) + '/';
    return RUN_ALL_TESTS();
}
