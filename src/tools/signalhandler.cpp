/*
    Copyright (C) 2014-2016 Islog

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
 * \file signalhandler.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief signal handler to provide a C++ interface for UNIX sigaction()
 */

#include "signalhandler.hpp"
#include <assert.h>

extern "C" {
#include <unistd.h>
}

#include "exception/signalexception.hpp"
#include "tools/unixsyscall.hpp"

using namespace Leosac::Tools;

static std::array<std::function<void(Signal)>, Leosac::Tools::num_signals>
    sigCallback;

static void fesser_e(int signal)
{
    assert(signal > 0 && signal < Leosac::Tools::num_signals);
    if (sigCallback[signal])
        sigCallback[signal](static_cast<Signal>(signal));
}

void SignalHandler::registerCallback(Signal signal,
                                     std::function<void(Signal)> callback)
{
    struct sigaction act;

    act.sa_handler = &fesser_e;
    if (sigemptyset(&act.sa_mask) < 0)
        throw(SignalException(UnixSyscall::getErrorString("sigemptyset", errno)));
    act.sa_flags = SA_RESTART;
    if (sigaction(static_cast<int>(signal), &act, 0) < 0)
        throw(SignalException(UnixSyscall::getErrorString("sigaction", errno)));
    assert(static_cast<int>(signal) > 0 &&
           static_cast<int>(signal) < Leosac::Tools::num_signals);
    sigCallback[static_cast<int>(signal)] = callback;
}
