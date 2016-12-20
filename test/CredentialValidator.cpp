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

#include "core/credentials/ICredential.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "exception/ModelException.hpp"
#include "gtest/gtest.h"

using namespace Leosac;
using namespace Leosac::Cred;

namespace Leosac
{
namespace Test
{

TEST(TestCredentialValidator, alias_length)
{
    RFIDCard c;

    ASSERT_NO_THROW({ c.alias("long_enough"); });

    ASSERT_NO_THROW({ c.alias("a"); });

    ASSERT_THROW(
        { c.alias("this_is_so_long_this_alias_is_clearly_to_long_to_be_valid"); },
        ModelException);

    std::string alias;
    for (int i = 0; i < 51; ++i)
    {
        ASSERT_NO_THROW(c.alias(alias));
        alias += '_';
    }
    ASSERT_THROW({ c.alias(alias); }, ModelException);
}

TEST(TestWiegandCardValidator, card_id)
{
    RFIDCard c;

    ASSERT_THROW({ c.card_id("11:22:33:4"); }, ModelException);
    ASSERT_THROW({ c.card_id("11:22:334"); }, ModelException);
    ASSERT_THROW({ c.card_id("lama"); }, ModelException);
    ASSERT_THROW({ c.card_id("1a:2-b"); }, ModelException);

    ASSERT_NO_THROW({ c.card_id("11:22:33:44"); });
    ASSERT_NO_THROW({ c.card_id("aa:bb:cc"); });
}

TEST(TestWiegandCardValidator, nb_bits)
{
    RFIDCard c;
    ASSERT_THROW(c.nb_bits(-1), ModelException);
    ASSERT_THROW(c.nb_bits(-42), ModelException);
    ASSERT_THROW(c.nb_bits(-1024), ModelException);

    ASSERT_NO_THROW(c.nb_bits(26));
    ASSERT_NO_THROW(c.nb_bits(37));
    ASSERT_NO_THROW(c.nb_bits(500)); // Makes no sense, but is not negative.
}

TEST(TestWiegandPinValidator, pin_is_numeric)
{
    PinCode c;
    ASSERT_THROW(c.pin_code("-1"), ModelException);
    ASSERT_THROW(c.pin_code("-42"), ModelException);
    ASSERT_THROW(c.pin_code("-1024"), ModelException);

    ASSERT_NO_THROW(c.pin_code("26"));
    ASSERT_NO_THROW(c.pin_code("37"));
    ASSERT_NO_THROW(c.pin_code("500"));
    ASSERT_NO_THROW(c.pin_code("12345678"));
}
}
}
