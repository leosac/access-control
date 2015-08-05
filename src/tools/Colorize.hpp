//
// Created by xaqq on 8/5/15.
//

#ifndef LEOSAC_COLORIZE_HPP
#define LEOSAC_COLORIZE_HPP

#include <string>

namespace Leosac
{
    /**
     * A collection of function to colorize / format text.
     */
    namespace Colorize
    {
        std::string bold(const std::string &in);

        std::string underline(const std::string &in);

        std::string red(const std::string &in);

        std::string green(const std::string &in);
    }
}
#endif //LEOSAC_COLORIZE_HPP
