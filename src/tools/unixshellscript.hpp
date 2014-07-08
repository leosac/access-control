/**
 * \file unixshellscript.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief UnixShellScript class declaration
 */

#ifndef UNIXSHELLSCRIPT_HPP
#define UNIXSHELLSCRIPT_HPP

#include <string>

class UnixShellScript
{
    static const std::size_t BufferSize = 1024;

public:
    explicit UnixShellScript(const std::string& script);
    ~UnixShellScript() = default;

    UnixShellScript(const UnixShellScript& other) = delete;
    UnixShellScript& operator=(const UnixShellScript& other) = delete;

public:
    int                 run(const std::string& args);
    const std::string&  getOutput() const;

public:
    const std::string   _script;
    std::string         _output;
};

#endif // UNIXSHELLSCRIPT_HPP
