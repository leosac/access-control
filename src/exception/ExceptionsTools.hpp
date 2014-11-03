#pragma once

#include <exception>

namespace Leosac
{
    /**
    * Recursively print the exception trace to std::cerr
    */
    void print_exception(const std::exception& e, int level =  0);
}