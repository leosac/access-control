#include "gtest/gtest.h"
#include <string>
#include "tools/dfautomaton.hpp"

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
