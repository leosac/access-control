/**
 * \file signalhandler.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#include "signalhandler.hpp"

#include <signal.h>
#include <unistd.h>

#include "isignalcallback.hpp"
#include "exception/signalexception.hpp"

static ISignalCallback* sigCallback = nullptr;

static void fesser_e(int signal)
{
    if (sigCallback)
        sigCallback->handleSignal(signal);
}

void SignalHandler::registerCallback(ISignalCallback* callback)
{
    struct sigaction    act;

    act.sa_handler = &fesser_e;
    if (sigemptyset(&act.sa_mask) == -1)
        throw (SignalException("sigemptyset() failed"));
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
    sigCallback = callback;
}
