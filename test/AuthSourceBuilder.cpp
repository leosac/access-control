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

#include "core/auth/AuthSourceBuilder.hpp"
#include "core/auth/Auth.hpp"
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "modules/auth/auth-file/FileAuthSourceMapper.hpp"
#include <gtest/gtest.h>

using namespace Leosac::Auth;
using namespace Leosac::Module::Auth;

namespace Leosac
{
namespace Test
{
/**
* Test the AuthSourceBuilder ability to create AuthSource object
* from messages.
*/
class AuthSourceBuilderTest : public ::testing::Test
{
  public:
    AuthSourceBuilderTest()
    {
        msg1_ << "S_MY_WIEGAND_1";
        msg1_ << SourceType::SIMPLE_WIEGAND;
        msg1_ << "af:bc:12:42";
        msg1_ << 32;

        msg2_ << "S_MY_WIEGAND_1";
        msg2_ << SourceType::WIEGAND_PIN;
        msg2_ << "1234";

        msg3_ << "S_MY_WIEGAND_1";
        msg3_ << SourceType::WIEGAND_CARD_PIN;
        msg3_ << "af:bc:12:42";
        msg3_ << 32;
        msg3_ << "1234";
    }

    ~AuthSourceBuilderTest()
    {
    }

    AuthSourceBuilder builder_;
    /**
    * This looks like a message sent by MY_WIEGAND_1 with SIMPLE_WIEGAND data.
    */
    zmqpp::message msg1_;

    /**
    * This looks like a message sent by MY_WIEGAND_1 with WIEGAND_PIN_4BITS data.
    */
    zmqpp::message msg2_;

    /**
    * Message to construct a cardId + pin credential.
    */
    zmqpp::message msg3_;
};

TEST_F(AuthSourceBuilderTest, ExtractSourceName)
{
    std::string out;

    ASSERT_TRUE(builder_.extract_source_name("S_MY_DEVICE", &out));
    ASSERT_EQ(out, "MY_DEVICE");

    ASSERT_TRUE(builder_.extract_source_name("S___MY_DEVICE", &out));
    ASSERT_EQ(out, "__MY_DEVICE");

    ASSERT_FALSE(builder_.extract_source_name("MY_DEVICE", &out));
    ASSERT_FALSE(builder_.extract_source_name("", &out));
    ASSERT_FALSE(builder_.extract_source_name("S_", &out));
    ASSERT_FALSE(builder_.extract_source_name("D", &out));
}

TEST_F(AuthSourceBuilderTest, BuildWiegandCard)
{
    Cred::ICredentialPtr auth_source = builder_.create(&msg1_);
    ASSERT_TRUE(auth_source.get());

    // ASSERT_EQ(auth_source->name(), "MY_WIEGAND_1");
    Cred::RFIDCardPtr spec = std::dynamic_pointer_cast<Cred::RFIDCard>(auth_source);
    ASSERT_TRUE(spec.get());
    ASSERT_EQ("af:bc:12:42", spec->card_id());
    ASSERT_EQ(32, spec->nb_bits());
}

TEST_F(AuthSourceBuilderTest, BuildPin)
{
    Cred::ICredentialPtr auth_source = builder_.create(&msg2_);
    ASSERT_TRUE(auth_source.get());

    // ASSERT_EQ(auth_source->name(), "MY_WIEGAND_1");
    Cred::PinCodePtr spec = std::dynamic_pointer_cast<Cred::PinCode>(auth_source);
    ASSERT_TRUE(spec.get());
    ASSERT_EQ("1234", spec->pin_code());
}

TEST_F(AuthSourceBuilderTest, BuildCardAndPing)
{
    Cred::ICredentialPtr auth_source = builder_.create(&msg3_);
    ASSERT_TRUE(auth_source.get());

    // ASSERT_EQ(auth_source->name(), "MY_WIEGAND_1");
    Cred::RFIDCardPinPtr spec =
        std::dynamic_pointer_cast<Cred::RFIDCardPin>(auth_source);
    ASSERT_TRUE(spec.get());
    ASSERT_EQ("af:bc:12:42", spec->card().card_id());
    ASSERT_EQ(32, spec->card().nb_bits());
    ASSERT_EQ("1234", spec->pin().pin_code());
}
}
}
