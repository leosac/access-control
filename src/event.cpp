/**
 * \file event.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief event class for intermodule communications
 */

#include "event.hpp"

#include <chrono>

Event::Event()
{
    date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

Event::~Event()
{

}
