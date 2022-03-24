/*
    Copyright (C) 2014-2022 Leosac

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

#include "ThreadUtils.hpp"
#include "enforce.hpp"
#include <errno.h>
#include <spdlog/fmt/fmt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace Leosac
{
uint64_t gettid()
{
    return static_cast<uint64_t>(syscall(SYS_gettid));
}

void set_thread_name(const std::string &name)
{
    int ret = prctl(PR_SET_NAME, name.c_str(), NULL, NULL, NULL);
    LEOSAC_ENFORCE(ret == 0,
                   fmt::format("Failed to set thread name. Errno {}", errno));
}
}
