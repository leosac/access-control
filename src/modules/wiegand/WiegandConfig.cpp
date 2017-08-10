/*
    Copyright (C) 2014-2017 Leosac

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

#include "modules/wiegand/WiegandConfig.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

WiegandConfig::WiegandConfig()
    : id_(0)
{
}

void WiegandConfig::add_reader(WiegandReaderConfig reader)
{
    readers_.push_back(reader);
}

void WiegandConfig::clear_reader()
{
    readers_.clear();
}

const std::vector<WiegandReaderConfig> &WiegandConfig::readers() const
{
    return readers_;
}

WiegandConfigId WiegandConfig::id() const
{
    return id_;
}
}
}
}