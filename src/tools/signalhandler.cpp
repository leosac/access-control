/**
 * \file signalhandler.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#include "signalhandler.hpp"

extern "C" {
#include <signal.h>
#include <unistd.h>
}

#include "exception/signalexception.hpp"
#include "tools/unixsyscall.hpp"

static std::function<void (int)> sigCallback;

static void fesser_e(int signal)
{
    if (sigCallback)
        sigCallback(signal);
}

void SignalHandler::registerCallback(std::function<void (int)> callback)
{
    struct sigaction    act;

    act.sa_handler = &fesser_e;
    if (sigemptyset(&act.sa_mask) < 0)
        throw (SignalException(UnixSyscall::getErrorString("sigemptyset", errno)));
    act.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &act, 0) < 0)
        throw (SignalException(UnixSyscall::getErrorString("sigaction", errno)));
    sigCallback = callback;
}
