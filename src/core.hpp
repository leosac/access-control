/**
 * \file core.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

class Core
{
public:
    Core();
    ~Core();

private:
    Core(const Core& other);
    Core& operator=(const Core& other);
};

#endif // CORE_HPP
