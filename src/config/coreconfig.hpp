/**
 * \file coreconfig.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief CoreConfig class declaration
 */

#ifndef CORECONFIG_HPP
#define CORECONFIG_HPP

#include <string>
#include <list>

class CoreConfig
{
public:
    typedef struct {
        std::string file;
        std::string alias;
    } Plugin;

public:
    explicit CoreConfig(const std::string& path);
    ~CoreConfig() = default;

    CoreConfig(const CoreConfig& other) = delete;
    CoreConfig& operator=(const CoreConfig& other) = delete;

public:
    void    load();
    void    save();

public:
    const std::list<std::string>&   getPluginDirs() const;
    const std::list<Plugin>&        getPlugins() const;

private:
    const std::string       _path;
    std::list<std::string>  _pluginDirs;
    std::list<Plugin>       _plugins;
};

#endif // CORECONFIG_HPP
