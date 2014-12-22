/**
* \file unixshellscript.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief UnixShellScript class declaration
*/

#ifndef UNIXSHELLSCRIPT_HPP
#define UNIXSHELLSCRIPT_HPP

#include <string>
#include <sstream>

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

            template<typename T>
            static std::string toCmdLine(T value)
            {
                std::ostringstream oss;

                oss << value;
                return (oss.str());
            }

            template<typename T, typename... Targs>
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
