/**
 * \file runtimeoptions.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief RuntimeOptions class declaration
 */

#ifndef RUNTIMEOPTIONS_HPP
#define RUNTIMEOPTIONS_HPP

#include <bitset>
#include <cstdint>

class RuntimeOptions
{
public:
    enum Flag {
        Verbose = 0,
        FlagCount
    };

public:
    explicit RuntimeOptions() = default;
    ~RuntimeOptions() = default;

    RuntimeOptions(const RuntimeOptions& other) = delete;
    RuntimeOptions& operator=(const RuntimeOptions& other) = delete;

public:
    void    setFlag(Flag flag, bool value = true);
    bool    isSet(Flag flag) const;

private:
    std::bitset<FlagCount>  _flags;
};

#endif // RUNTIMEOPTIONS_HPP
