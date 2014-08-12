/**
 * \file signalhandler.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <functional>

extern "C" {
    #include <signal.h>
}

enum class Signal : int {
    SigHup = SIGHUP,
    SigInt = SIGINT,
    SigQuit = SIGQUIT,
    SigKill = SIGKILL,
    SigSegv = SIGSEGV,
    SigPipe = SIGPIPE,
    SigTerm = SIGTERM,
    SigUsr1 = SIGUSR1,
    SigUsr2 = SIGUSR2,
    SigStop = SIGSTOP
};

class SignalHandler
{
    SignalHandler() = delete;

public:
    static void registerCallback(Signal signal, std::function<void (Signal)> callback);
};

#endif // SIGNALHANDLER_HPP

