/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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

namespace Leosac
{
    namespace Tools
    {
        enum class Signal : int
        {
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

        static constexpr int num_signals = _NSIG;

        class SignalHandler
        {
            SignalHandler() = delete;

        public:
            static void registerCallback(Signal signal, std::function<void(Signal)> callback);
        };

    }
}

#endif // SIGNALHANDLER_HPP

