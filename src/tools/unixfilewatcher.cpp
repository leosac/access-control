/**
 * \file unixfilewatcher.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief UnixFileWatcher class implementation
 */

#include "unixfilewatcher.hpp"

extern "C" {
#include <sys/inotify.h>
#include <unistd.h>
}

#include "exception/fsexception.hpp"
#include "tools/unixsyscall.hpp"
#include "tools/log.hpp"

UnixFileWatcher::UnixFileWatcher()
:   _isRunning(false)
{
    if ((_inotifyFd = inotify_init()) == -1)
        throw (FsException(UnixSyscall::getErrorString("inotify_init", errno)));
}

UnixFileWatcher::~UnixFileWatcher()
{
    try {
        if (close(_inotifyFd) == -1)
            throw (FsException(UnixSyscall::getErrorString("close", errno)));
    }
    catch (const FsException& e) {
        LOG() << "Exception caught: " << e.what();
    }
}

void UnixFileWatcher::start()
{
    _isRunning = true;
    _thread = std::thread([this] () {
        run();
    } );
}

void UnixFileWatcher::stop()
{
    _isRunning = false;
    _thread.join();
    for (auto watch : _watches)
    {
        if (inotify_rm_watch(_inotifyFd, watch.first) == -1)
            throw (FsException(UnixSyscall::getErrorString("inotify_rm_watch", errno)));
    }
    _watches.clear();
}

void UnixFileWatcher::watchFile(const std::string& path)
{
    std::uint32_t   mask = IN_MODIFY;
    WatchParams     params;
    UnixFd          watch;

    if ((watch = inotify_add_watch(_inotifyFd, path.c_str(), mask)) == -1)
        throw (FsException(UnixSyscall::getErrorString("inotify_add_watch", errno)));
    params.path = path;
    params.mask = 0;
    _watches[watch] = params;
}

bool UnixFileWatcher::fileHasChanged(const std::string& path) const
{
    for (auto& param : _watches)
    {
        if (param.second.path == path)
            return ((param.second.mask & IN_MODIFY) > 0);
    }
    throw (FsException("no registered watch for path:" + path));
}

void UnixFileWatcher::fileReset(const std::string& path)
{
    for (auto& param : _watches)
    {
        if (param.second.path == path)
        {
            param.second.mask = 0;
            return;
        }
    }
    throw (FsException("no registered watch for path:" + path));
}

std::size_t UnixFileWatcher::size() const
{
    return (_watches.size());
}

void UnixFileWatcher::run()
{
    struct timeval  timeoutStruct;
    int             ret;
    long            timeout = DefaultTimeoutMs;
    fd_set          readSet;
    std::size_t     buflen = 1024 * (sizeof(struct inotify_event) + 16);
    char            buf[buflen];
    int             len;
    inotify_event*  event;

    while (_isRunning)
    {
        FD_ZERO(&readSet);
        FD_SET(_inotifyFd, &readSet);
        timeoutStruct.tv_sec = timeout / 1000;
        timeoutStruct.tv_usec = (timeout % 1000) * 1000;
        if ((ret = ::select(_inotifyFd + 1, &readSet, nullptr, nullptr, &timeoutStruct)) == -1)
            throw (FsException(UnixSyscall::getErrorString("select", errno)));
        else if (ret > 0)
        {
            if (!FD_ISSET(_inotifyFd, &readSet))
                throw (FsException("unexpected file descriptor set"));
            if ((len = read(_inotifyFd, buf, buflen)) == -1)
            {
                if (errno != EINTR)
                    throw (FsException(UnixSyscall::getErrorString("read", errno)));
            }
            else if (!len)
                throw (FsException("nothing was read"));
            else
            {
                for (int i = 0; i < len;)
                {
                    event = reinterpret_cast<inotify_event*>(&buf[i]);
                    _watches.at(event->wd).mask |= event->mask;
                    i += sizeof(inotify_event) + event->len;
                }
            }
        }
    }
}
