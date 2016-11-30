/*
    Copyright (C) 2014-2016 Leosac

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
* \file unixfilewatcher.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief UnixFileWatcher class declaration
*/

#ifndef UNIXFILEWATCHER_HPP
#define UNIXFILEWATCHER_HPP

#include <atomic>
#include <map>
#include <string>
#include <thread>

namespace Leosac
{
namespace Tools
{

class UnixFileWatcher
{
    using UnixFd = int;
    struct WatchParams
    {
        std::string path;
        int mask;
    };
    using Watches = std::map<UnixFd, WatchParams>;

    static const long DefaultTimeoutMs = 2000;

  public:
    explicit UnixFileWatcher();

    ~UnixFileWatcher();

    UnixFileWatcher(const UnixFileWatcher &other) = delete;

    UnixFileWatcher &operator=(const UnixFileWatcher &other) = delete;

  public:
    void start();

    void stop();

  public:
    void watchFile(const std::string &path);

    bool fileHasChanged(const std::string &path) const;

    void fileReset(const std::string &path);

    std::size_t size() const;

  private:
    void run();

  private:
    std::thread _thread;
    std::atomic<bool> _isRunning;
    UnixFd _inotifyFd;
    Watches _watches;
};
}
}

#endif // UNIXFILEWATCHER_HPP
