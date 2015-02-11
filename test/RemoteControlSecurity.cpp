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
#include "core/RemoteControlSecurity.hpp"
#include <gtest/gtest.h>
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
        class RemoteControlSecurityTest : public ::testing::Test
        {
        public:
            RemoteControlSecurityTest()
            {
              //  cfg0 = std::make_shared<ConfigManager>(boost::property_tree::ptree());

                auto cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path + "RemoteControlSecurity1.xml");
                sec1 = std::make_shared<RemoteControlSecurity>(cfg_tree.get_child("kernel.remote"));
            }

        protected:
            std::shared_ptr<RemoteControlSecurity> sec1;
        };


        TEST_F(RemoteControlSecurityTest, Test1)
        {
            ASSERT_TRUE(sec1->allow_request("Llama", "SYNC_FROM"));
            ASSERT_TRUE(sec1->allow_request("Llama", "MODULE_CONFIG"));
            // this doesn't exist, but security code doesn't care.
            ASSERT_TRUE(sec1->allow_request("Llama", "HAHAHAHAHHA"));

            ASSERT_FALSE(sec1->allow_request("Worm", "SYNC_FROM"));
            ASSERT_TRUE(sec1->allow_request("Worm", "MODULE_CONFIG"));
            ASSERT_FALSE(sec1->allow_request("Worm", "HAHAHAHAHHA"));

            ASSERT_FALSE(sec1->allow_request("LlamaWorm", "SYNC_FROM"));
            ASSERT_FALSE(sec1->allow_request("LlamaWorm", "MODULE_CONFIG"));
            ASSERT_FALSE(sec1->allow_request("LlamaWorm", "HAHAHAHAHHA"));
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

