/**
 * \file unixfs.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief unix filesystem helper functions
 */

#ifndef UNIXFS_HPP
#define UNIXFS_HPP

#include <string>
#include <list>

class UnixFs
{
    UnixFs();
public:
    typedef std::list<std::string> FileList;

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
};

#endif // UNIXFS_HPP
