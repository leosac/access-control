#include <exception>
#include <iostream>
#include <tools/log.hpp>
#include "ExceptionsTools.hpp"

void Leosac::print_exception(const std::exception &e, int level /* = 0 */)
{
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception &e2)
    {
        print_exception(e2, level + 1);
    }
    catch (...)
    {
        std::cerr << "unkown exception type" << std::endl;
    }
}

void Leosac::log_exception(const std::exception &e, int level /* = 0 */)
{
    ERROR(std::string(level, ' ') << "exception: " << e.what());
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception &e2)
    {
        print_exception(e2, level + 1);
    }
    catch (...)
    {
        ERROR("unkown exception type");
    }
}

