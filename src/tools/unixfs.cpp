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
 * \file unixfs.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief unix filesystem helper functions
 */

#include "unixfs.hpp"

extern "C" {
#include <unistd.h>
#include <dirent.h>
}

#include <cerrno>
#include <algorithm>

#include "unixsyscall.hpp"

using namespace Leosac::Tools;

std::string UnixFs::getCWD()
{
    char *str = getcwd(nullptr, 0);
    std::string path;

    if (str)
    {
        path = str;
        free(str);
        return (path);
    }
    else
        return (".");
}

UnixFs::FileList UnixFs::listFiles(const std::string &folder,
                                   const std::string &extension)
{
    FileList l;
    DIR *dir;
    struct dirent *entry;
    std::string filename;
    std::string path = folder;

    if (path.empty())
        throw(FsException("empty folder"));
    if (*path.rbegin() != '/')
        path.append("/");
    if ((dir = opendir(path.c_str())) == nullptr)
        throw(FsException(UnixSyscall::getErrorString("opendir", errno)));
    errno = 0;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type != DT_REG)
            continue;
        filename = entry->d_name;
        if (filename.size() < extension.size())
            continue;
        if (std::equal(extension.rbegin(), extension.rend(), filename.rbegin()))
            l.push_back(path + filename);
    }
    if (errno)
        throw(FsException(UnixSyscall::getErrorString("readdir", errno)));
    if (closedir(dir) == -1)
        throw(FsException(UnixSyscall::getErrorString("closedir", errno)));
    return (l);
}

std::string UnixFs::stripPath(const std::string &filename)
{
    std::size_t pos;

    if ((pos = filename.find_last_of('/')) == std::string::npos)
        return (filename);
    else
        return (filename.substr(pos + 1));
}

std::string UnixFs::readAll(const std::string &path)
{
    std::ifstream file(path);

    if (!file.good())
        throw(FsException("could not open " + path + '\''));
    return (std::string(
        std::istreambuf_iterator<char>(static_cast<std::istream &>(file)),
        std::istreambuf_iterator<char>()));
}

bool UnixFs::fileExists(const std::string &path)
{
    if (access(path.c_str(), F_OK) == -1)
    {
        if (errno == ENOENT)
            return (false);
        throw(FsException(UnixSyscall::getErrorString("access", errno)));
    }
    else
        return (true);
}
