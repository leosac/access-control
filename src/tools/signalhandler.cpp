/**
 * \file signalhandler.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#include "signalhandler.hpp"

extern "C" {
#include <unistd.h>
}

#include "exception/signalexception.hpp"
#include "tools/unixsyscall.hpp"

static std::function<void (Signal)> sigCallback;

static void fesser_e(int signal)
{
    if (sigCallback)
        sigCallback(static_cast<Signal>(signal));
}

void SignalHandler::registerCallback(Signal signal, std::function<void (Signal)> callback)
{
    struct sigaction    act;

    act.sa_handler = &fesser_e;
    if (sigemptyset(&act.sa_mask) < 0)
        throw (SignalException(UnixSyscall::getErrorString("sigemptyset", errno)));
    act.sa_flags = SA_RESTART;
    if (sigaction(static_cast<int>(signal), &act, 0) < 0)
        throw (SignalException(UnixSyscall::getErrorString("sigaction", errno)));
    sigCallback = callback;
}
