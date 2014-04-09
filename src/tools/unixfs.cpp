/**
 * \file unixfs.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief unix filesystem helper functions
 */

#include "unixfs.hpp"

#include <unistd.h>
#include <dirent.h>

#include <cerrno>
#include <algorithm>

#include "unixsyscall.hpp"
#include "exception/fsexception.hpp"

UnixFs::UnixFs() {}

std::string UnixFs::getCWD()
{
    char*       str = getcwd(NULL, 0);
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

UnixFs::FileList UnixFs::listFiles(const std::string& folder, const std::string& extension)
{
    FileList        l;
    DIR*            dir;
    struct dirent*  entry;
    std::string     filename;
    std::string     path = folder;

    if (path.empty())
        throw (FsException("empty folder"));
    if (*path.rbegin() != '/')
        path.append("/");
    if ((dir = opendir(path.c_str())) == NULL)
        throw (FsException(UnixSyscall::getErrorString("opendir", errno)));
    while((entry = readdir(dir)) != NULL)
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
        throw (FsException(UnixSyscall::getErrorString("readdir", errno)));
    if (closedir(dir) == -1)
        throw (FsException(UnixSyscall::getErrorString("closedir", errno)));
    return (l);
}
