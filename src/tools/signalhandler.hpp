/**
 * \file signalhandler.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <functional>

class SignalHandler
{
    SignalHandler() = delete;

public:
    static void registerCallback(std::function<void (int)> callback);
};

#endif // SIGNALHANDLER_HPP
