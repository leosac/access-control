#pragma once

#include <exception>

namespace Leosac
{
    /**
    * Recursively print the exception trace to std::cerr
    */
    void print_exception(const std::exception& e, int level =  0);

    /**
    * Recursively log exceptions using the logging macro.
    */
    void log_exception(const std::exception& e, int level =  0);
}