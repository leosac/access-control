/**
 * \file main.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief standard main
 */

#include <iostream>

#include "exception/osacexception.hpp"
#include "core/core.hpp"

int main(int ac, char** av)
{
    std::list<std::string>  args;

    for (int i = 1; i < ac; ++i)
        args.push_back(av[i]);
    try
    {
        Core    core;

        core.run(args);
    }
    catch (const OSACException& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
