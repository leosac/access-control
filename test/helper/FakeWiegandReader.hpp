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