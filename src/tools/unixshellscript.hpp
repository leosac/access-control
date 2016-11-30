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

/**
* \file unixshellscript.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief UnixShellScript class declaration
*/

#ifndef UNIXSHELLSCRIPT_HPP
#define UNIXSHELLSCRIPT_HPP

#include <sstream>
#include <string>

namespace Leosac
{
namespace Tools
{
class UnixShellScript
{
    static const std::size_t BufferSize = 1024;

  public:
    explicit UnixShellScript(const std::string &script);

    ~UnixShellScript() = default;

    UnixShellScript(const UnixShellScript &other) = delete;

    UnixShellScript &operator=(const UnixShellScript &other) = delete;

  public:
    int run(const std::string &args = std::string());

    const std::string &getOutput() const;

    template <typename T>
    static std::string toCmdLine(T value)
    {
        std::ostringstream oss;

        oss << value;
        return (oss.str());
    }

    template <typename T, typename... Targs>
    static std::string toCmdLine(T value, Targs... args)
    {
        if (sizeof...(args) > 0)
            return (toCmdLine(value) + ' ' + toCmdLine(args...));
    }

  private:
    const std::string _script;
    std::string _output;
};
}
}

#endif // UNIXSHELLSCRIPT_HPP
