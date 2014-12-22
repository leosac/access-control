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
#include <string>
#include "tools/dfautomaton.hpp"

using namespace Leosac::Tools;

namespace Leosac
{
    namespace Test
    {
        TEST(AutomatonTest, SimpleGraph)
        {
            DFAutomaton<std::string, int, int> dfa;
            int state;
            std::string val;

            dfa.addNode(1, [](std::string &val)
            {
                val = "I";
            });
            dfa.addNode(2, [](std::string &val)
            {
                val += " f*cking";
            });
            dfa.addNode(3, [](std::string &val)
            {
                val += " hate";
            });
            dfa.addNode(4, [](std::string &val)
            {
                val += " love";
            });
            dfa.addNode(5, [](std::string &val)
            {
                val += " c++";
            });
            dfa.addTransition(1, 1, 2);
            dfa.addTransition(2, 1, 4);
            dfa.addTransition(2, 2, 3);
            dfa.addTransition(3, 1, 5);
            dfa.addTransition(4, 1, 5);

            state = 1;
            dfa.startNode(val, state);
            state = dfa.update(val, state, 1);
            state = dfa.update(val, state, 1);
            state = dfa.update(val, state, 1);

            EXPECT_EQ("I f*cking love c++", val);

            state = 1;
            dfa.startNode(val, state);
            state = dfa.update(val, state, 1);
            state = dfa.update(val, state, 2);
            state = dfa.update(val, state, 1);

            EXPECT_EQ("I f*cking hate c++", val);
        }
    }
}
