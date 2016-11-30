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
 * \file unixshellscript.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief UnixShellScript class implementation
 */

#include "unixshellscript.hpp"

extern "C" {
#include <stdio.h>
}

#include <sstream>

#include "exception/scriptexception.hpp"
#include "tools/log.hpp"
#include "unixfs.hpp"
#include "unixsyscall.hpp"

using namespace Leosac::Tools;

UnixShellScript::UnixShellScript(const std::string &script)
    : _script(script)
{
}

int UnixShellScript::run(const std::string &args)
{
    FILE *stream;
    std::ostringstream oss;
    char buffer[BufferSize];
    std::string line = _script + ' ' + args;
    int ret;

    INFO("CmdLine: " << line);
    if (!(stream = popen(line.c_str(), "r")))
        throw(ScriptException(UnixSyscall::getErrorString("popen", errno) +
                              " command: '" + line + '\''));
    while (fgets(buffer, BufferSize, stream))
        oss << buffer;
    _output = oss.str();
    if ((ret = pclose(stream)) == -1 && errno)
        throw(ScriptException(UnixSyscall::getErrorString("pclose", errno)));
    return (ret);
}

const std::string &UnixShellScript::getOutput() const
{
    return (_output);
}
