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
* \file unixfs.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief unix filesystem helper functions
*/

#ifndef UNIXFS_HPP
#define UNIXFS_HPP

#include <string>
#include <list>
#include <fstream>

#include "exception/fsexception.hpp"

namespace Leosac
{
namespace Tools
{
class UnixFs
{
    UnixFs() = delete;

  public:
    using FileList = std::list<std::string>;

  public:
    /**
    * get current working directory without trailing slash
    * @return path
    */
    static std::string getCWD();

    /**
    * list all files with the extension ".extension" in folder
    * @param folder
    * @param extension name without the dot
    * @return file list
    */
    static FileList listFiles(const std::string &folder,
                              const std::string &extension = std::string());

    /**
    * remove the full path from a filename
    * @param filename to strip
    * @return filename without path
    */
    static std::string stripPath(const std::string &filename);

    /**
    * read all file contents and put it in a string
    * @param path Path of the file
    * @return file contents
    */
    static std::string readAll(const std::string &path);

    /**
    * @param path Path of the file
    * @return true is file exists
    */
    static bool fileExists(const std::string &path);

    /**
    * read value from a sysfs file
    * @param path Path of the sysfs target
    * @return Value read
    */
    template <typename T>
    static T readSysFsValue(const std::string &path)
    {
        std::ifstream file(path);
        T val;

        if (!file.good())
            throw(FsException("could not open \'" + path + '\''));
        file >> val;
        file.clear();
        file.seekg(0);
        return (val);
    }

    /**
    * write value to a sysfs file
    * @param path Path of the sysfs target
    * @param val Value to write
    */
    template <typename T>
    static void writeSysFsValue(const std::string &path, const T &val)
    {
        std::ofstream file(path);

        if (!file.good())
            throw(FsException("could not open " + path + '\''));
        file << val;
        file.clear();
        file.seekp(0);
    }
};
}
}

#endif // UNIXFS_HPP
