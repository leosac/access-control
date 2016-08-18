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

#include "GenGuid.h"
#include "tools/log.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

std::string Leosac::gen_uuid()
{
    ssize_t ret;
    int fd;
    fd = open("/proc/sys/kernel/random/uuid", 0);
    ASSERT_LOG(fd != 1, "Cannot open /proc/sys/kernel/random/uuid");

    std::array<char, 360> uuid;
    ret = read(fd, uuid.data(), 36);
    close(fd);
    ASSERT_LOG(ret == 36, "Cannot read UUID from /proc/sys/kernel/random/uuid");

    std::string uuid_str(uuid.data(), 36);
    return uuid_str;
}
