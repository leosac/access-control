/**
 * \file runtimeoptions.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief RuntimeOptions class declaration
 */

#ifndef RUNTIMEOPTIONS_HPP
#define RUNTIMEOPTIONS_HPP

#include <bitset>
#include <cstdint>
#include <string>
#include <map>

class RuntimeOptions
{
    static const std::string    DefaultEmptyParam;

public:
    enum Flag {
        Verbose = 0,
        TestRun,
        FlagCount
    };

public:
    explicit RuntimeOptions() = default;
    ~RuntimeOptions() = default;

    RuntimeOptions(const RuntimeOptions& other) = delete;
    RuntimeOptions& operator=(const RuntimeOptions& other) = delete;

public:
    void                setFlag(Flag flag, bool value = true);
    bool                isSet(Flag flag) const;
    void                setParam(const std::string& key, const std::string& value);
    const std::string&  getParam(const std::string& key) const;

private:
    std::bitset<FlagCount>              _flags;
    std::map<std::string, std::string>  _params;
};

#endif // RUNTIMEOPTIONS_HPP
