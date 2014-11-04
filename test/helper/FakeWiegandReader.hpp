#pragma once

#include "zmqpp/zmqpp.hpp"

namespace Leosac
{
    namespace Test
    {
        namespace Helper
        {
            /**
            * A test helper that fake a WiegandReader (it read command on a socket and send OK)
            */
            class FakeWiegandReader
            {
            public:
                FakeWiegandReader(zmqpp::context &ctx, const std::string &name);

                FakeWiegandReader(const FakeWiegandReader &) = delete;

                /**
                * Runs the dummy ready in a thread.
                */
                bool run(zmqpp::socket *pipe);

            private:
                /**
                * Receive command here.
                */
                zmqpp::socket rep_;
            };
        }
    }
}