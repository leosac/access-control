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
    static std::string  getCWD();

    /**
     * list all files with the extension ".extension" in folder
     * @param folder
     * @param extension name without the comma
     * @return file list
     */
    static FileList     listFiles(const std::string& folder, const std::string& extension = std::string());

    /**
     * remove the full path from a filename
     * @param filename to strip
     * @return filename without path
     */
    static std::string  stripPath(const std::string& filename);

    /**
     * read all file contents and put it in a string
     * @param path Path of the file
     * @return file contents
     */
    static std::string  readAll(const std::string& path);

    /**
     * @param path Path of the file
     * @return true is file exists
     */
    static bool         fileExists(const std::string& path);

    /**
     * read value from a sysfs file
     * @param path Path of the sysfs target
     * @return Value read
     */
    template <typename T>
    static T    readSysFsValue(const std::string& path)
    {
        std::ifstream   file(path);
        T               val;

        if (!file.good())
            throw (FsException("could not open \'" + path + '\''));
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
    static void writeSysFsValue(const std::string& path, const T& val)
    {
        std::ofstream   file(path);

        if (!file.good())
            throw (FsException("could not open " + path + '\''));
        file << val;
        file.clear();
        file.seekp(0);
    }
};

#endif // UNIXFS_HPP
