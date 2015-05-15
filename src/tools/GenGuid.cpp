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

#include "GenGuid.h"
#include "tools/log.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

std::string Leosac::gen_uuid()
{
    int ret;
    int fd;
    char *uuid;
    fd = open("/proc/sys/kernel/random/uuid", 0);
    ASSERT_LOG(fd != 1, "Cannot open /proc/sys/kernel/random/uuid");

    uuid = static_cast<char *>(malloc(37));
    ASSERT_LOG(uuid, "Cannot allocate memory for UUID");

    ret = read(fd, uuid, 36);
    uuid[36] = 0;
    ASSERT_LOG(ret == 36, "Cannot read UUID from /proc/sys/kernel/random/uuid");

    std::string uuid_str(uuid);
    free(uuid);
    return uuid_str;
}
