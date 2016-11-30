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

#include "SysFsGpioConfig.hpp"
#include "tools/Colorize.hpp"
#include "tools/PropertyTreeExtractor.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

using namespace Leosac::Module::SysFsGpio;

SysFsGpioConfig::SysFsGpioConfig(const boost::property_tree::ptree &cfg)
{
    Tools::PropertyTreeExtractor extractor(cfg, "SysFsGpio");

    cfg_export_path_    = extractor.get<std::string>("export_path");
    cfg_unexport_path_  = extractor.get<std::string>("unexport_path");
    cfg_value_path_     = extractor.get<std::string>("value_path");
    cfg_edge_path_      = extractor.get<std::string>("edge_path");
    cfg_direction_path_ = extractor.get<std::string>("direction_path");

    auto aliases_cfg = cfg.get_child("aliases");
    for (auto alias : aliases_cfg)
    {
        if (alias.first == "default")
        {
            default_aliases_ = alias.second.data();
            continue;
        }

        int pin_no           = std::stoi(alias.first);
        pin_aliases_[pin_no] = alias.second.data();
    }

    using namespace Colorize;
    INFO("SysFsGpio Path Configuration:"
         << std::endl
         << '\t' << "Export path: " << green(cfg_export_path_) << std::endl
         << '\t' << "Unexport path: " << green(cfg_unexport_path_) << std::endl
         << '\t' << "Value path: " << green(cfg_value_path_) << std::endl
         << '\t' << "Edge path: " << green(cfg_edge_path_) << std::endl
         << '\t' << "Direction path: " << green(cfg_direction_path_) << std::endl
         << '\t' << "Default aliases rule: " << green(default_aliases_)
         << std::endl);
}

const std::string &SysFsGpioConfig::export_path() const
{
    return cfg_export_path_;
}

const std::string &SysFsGpioConfig::unexport_path() const
{
    return cfg_unexport_path_;
}

std::string SysFsGpioConfig::value_path(int pin_no) const
{
    if (pin_aliases_.count(pin_no))
        return boost::replace_all_copy(cfg_value_path_, "__PLACEHOLDER__",
                                       pin_aliases_.find(pin_no)->second);
    return boost::replace_all_copy(
        cfg_value_path_, "__PLACEHOLDER__",
        boost::replace_all_copy(default_aliases_, "__NO__", std::to_string(pin_no)));
}

std::string SysFsGpioConfig::edge_path(int pin_no) const
{
    if (pin_aliases_.count(pin_no))
        return boost::replace_all_copy(cfg_edge_path_, "__PLACEHOLDER__",
                                       pin_aliases_.find(pin_no)->second);
    return boost::replace_all_copy(
        cfg_edge_path_, "__PLACEHOLDER__",
        boost::replace_all_copy(default_aliases_, "__NO__", std::to_string(pin_no)));
}

std::string SysFsGpioConfig::direction_path(int pin_no) const
{
    if (pin_aliases_.count(pin_no))
        return boost::replace_all_copy(cfg_direction_path_, "__PLACEHOLDER__",
                                       pin_aliases_.find(pin_no)->second);
    return boost::replace_all_copy(
        cfg_direction_path_, "__PLACEHOLDER__",
        boost::replace_all_copy(default_aliases_, "__NO__", std::to_string(pin_no)));
}
