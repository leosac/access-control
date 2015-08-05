//
// Created by xaqq on 8/5/15.
//

#include "Colorize.hpp"

using namespace Leosac;

/**
 * Clear the formatting.
 */
static std::string clear()
{
    return "\033[0m";
}

std::string Colorize::underline(const std::string &in)
{
    return "\033[4m" + in + clear();
}

std::string Colorize::red(const std::string &in)
{
    return "\033[31m" + in + clear();
}

std::string Colorize::green(const std::string &in)
{
    return "\033[32m" + in + clear();
}

std::string Colorize::bold(const std::string &in)
{
    return "\033[1m" + in + clear();
}
