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

#include "core/RemoteControlSecurity.hpp"
#include "tools/XmlPropertyTree.hpp"
#include <gtest/gtest.h>
#include <string>

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

        auto cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path +
                                                       "RemoteControlSecurity1.xml");
        sec1 = std::make_shared<RemoteControlSecurity>(
            cfg_tree.get_child("kernel.remote"));

        cfg_tree = Tools::propertyTreeFromXmlFile(gl_data_path +
                                                  "RemoteControlSecurity2.xml");
        sec2 = std::make_shared<RemoteControlSecurity>(
            cfg_tree.get_child("kernel.remote"));
    }

  protected:
    std::shared_ptr<RemoteControlSecurity> sec1;
    std::shared_ptr<RemoteControlSecurity> sec2;
};


TEST_F(RemoteControlSecurityTest, Test1)
{
    ASSERT_TRUE(sec1->allow_request("Llama", "SYNC_FROM"));
    ASSERT_TRUE(sec1->allow_request("Llama", "MODULE_CONFIG"));
    // this doesn't exist, but security code doesn't care.
    ASSERT_TRUE(sec1->allow_request("Llama", "HAHAHAHAHHA"));

    ASSERT_FALSE(sec1->allow_request("Worm", "SYNC_FROM"));
    ASSERT_TRUE(sec1->allow_request("Worm", "MODULE_CONFIG"));
    ASSERT_TRUE(sec1->allow_request("Worm", "MODULE_LIST"));
    ASSERT_FALSE(sec1->allow_request("Worm", "HAHAHAHAHHA"));

    ASSERT_FALSE(sec1->allow_request("LlamaWorm", "SYNC_FROM"));
    ASSERT_FALSE(sec1->allow_request("LlamaWorm", "MODULE_CONFIG"));
    ASSERT_FALSE(sec1->allow_request("LlamaWorm", "HAHAHAHAHHA"));
}

/**
* Test default setting, when no <security> tag is present.
* This should allow everyone to do everything.
*/
TEST_F(RemoteControlSecurityTest, TestNoSecurityInformation)
{
    ASSERT_TRUE(sec2->allow_request("Llama", "SYNC_FROM"));
    ASSERT_TRUE(sec2->allow_request("Llama", "MODULE_CONFIG"));
    // this doesn't exist, but security code doesn't care.
    ASSERT_TRUE(sec2->allow_request("Llama", "HAHAHAHAHHA"));

    ASSERT_TRUE(sec2->allow_request("Worm", "SYNC_FROM"));
    ASSERT_TRUE(sec2->allow_request("Worm", "MODULE_CONFIG"));
    ASSERT_TRUE(sec1->allow_request("Worm", "MODULE_LIST"));
    ASSERT_TRUE(sec2->allow_request("Worm", "HAHAHAHAHHA"));

    ASSERT_TRUE(sec2->allow_request("LlamaWorm", "SYNC_FROM"));
    ASSERT_TRUE(sec2->allow_request("LlamaWorm", "MODULE_CONFIG"));
    ASSERT_TRUE(sec2->allow_request("LlamaWorm", "HAHAHAHAHHA"));
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
