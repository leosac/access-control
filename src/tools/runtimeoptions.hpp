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

/**
* \file runtimeoptions.hpp
* \brief RuntimeOptions class declaration
*/

#ifndef RUNTIMEOPTIONS_HPP
#define RUNTIMEOPTIONS_HPP

#include <bitset>
#include <cstdint>
#include <map>
#include <string>

namespace Leosac
{
namespace Tools
{
/**
* Holds informations about runtime options, such as "is this a verbose run" or path
* to configurations files
*/
class RuntimeOptions
{
    static const std::string DefaultEmptyParam;

  public:
    explicit RuntimeOptions();

    ~RuntimeOptions() = default;

    RuntimeOptions(const RuntimeOptions &other) = delete;

    RuntimeOptions &operator=(const RuntimeOptions &other) = delete;

  public:
    bool has_param(const std::string &key) const;

    void set_param(const std::string &key, const std::string &value);

    const std::string &get_param(const std::string &key) const;

    bool is_strict() const;
    void set_strict(bool v);

  private:
    std::map<std::string, std::string> _params;
    bool strict_;
};
}
}

#endif // RUNTIMEOPTIONS_HPP
