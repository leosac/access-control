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

#include "LibgpiodConfig.hpp"
#include "tools/Colorize.hpp"
#include "tools/PropertyTreeExtractor.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

using namespace Leosac::Module::Libgpiod;

LibgpiodConfig::LibgpiodConfig(const boost::property_tree::ptree &cfg)
{
    Tools::PropertyTreeExtractor extractor(cfg, "Libgpiod");

    consumer_ = extractor.get<std::string>("consumer");

    using namespace Colorize;
    INFO("Libgpiod Configuration:"
         << std::endl
         << '\t' << "Consumer: " << green(consumer_)
         << std::endl);
}

const std::string &LibgpiodConfig::consumer() const
{
    return consumer_;
}
