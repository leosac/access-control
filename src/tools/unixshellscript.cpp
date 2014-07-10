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

#include "unixfs.hpp"
#include "unixsyscall.hpp"
#include "tools/log.hpp"
#include "exception/scriptexception.hpp"

UnixShellScript::UnixShellScript(const std::string& script)
:   _script(script)
{}

int UnixShellScript::run(const std::string& args)
{
    FILE*               stream;
    std::ostringstream  oss;
    char                buffer[BufferSize];
    std::string         line = _script + ' ' + args;
    int                 ret;

    LOG() << "CmdLine: " << line;
    if (!(stream = popen(line.c_str(), "r")))
        throw (ScriptException(UnixSyscall::getErrorString("popen", errno) + " command: '" + line + '\''));
    while (fgets(buffer, BufferSize, stream))
        oss << buffer;
    _output = oss.str();
    if ((ret = pclose(stream)) == -1 && errno)
        throw (ScriptException(UnixSyscall::getErrorString("pclose", errno)));
    return (ret);
}

const std::string& UnixShellScript::getOutput() const
{
    return (_output);
}

