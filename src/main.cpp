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
* \file main.cpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief standard main
*/

#include <iostream>
// NOTE TCLAP is easily replacable by boost::program_options
#include <tclap/CmdLine.h>
#include <unistd.h>
#include "core/kernel.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/log.hpp"
#include "tools/leosac.hpp"
#include "tools/unixshellscript.hpp"

using namespace Leosac::Tools;
using namespace Leosac;

static int set_working_directory(RuntimeOptions &opts) noexcept
{
    int ret = 0;
    if (opts.hasParam("working_directory") && !opts.getParam("working_directory").empty())
    {
        ret = chdir(opts.getParam("working_directory").c_str());
        if (ret != 0)
            perror("Cannot change working directory");
    }
    return ret;
}

static std::string my_getpwd()
{
    std::unique_ptr<char, void(*)(void *)> pwd(get_current_dir_name(), free);
    assert(pwd);

    return std::string(pwd.get());
}

int main(int argc, const char **argv)
{
    RuntimeOptions options;
    int relaunch = 1;

    try
    {
        TCLAP::CmdLine cmd("Open Source Access Controller", ' ', Leosac::getVersionString());
        TCLAP::SwitchArg verboseSwitch("v", "verbose", "Increase verbosity", false);
        TCLAP::SwitchArg strict("s", "strict", "Be strict regarding configuration error", false); // assert on configuration error.
        TCLAP::ValueArg<std::string> kernelFile("k", "kernel-cfg", "Kernel Configuration file", true, "", "config_file");
        TCLAP::ValueArg<std::string> working_directory("d", "working-directory", "Leosac's working directory", false, "", "working_directory");

        cmd.add(verboseSwitch);
        cmd.add(strict);
        cmd.add(kernelFile);
        cmd.add(working_directory);
        cmd.parse(argc, argv);
        options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
        options.setParam("kernel-cfg", kernelFile.getValue());
        options.setParam("working_directory", working_directory.getValue());
        options.set_strict(strict.getValue());
    }
    catch (const TCLAP::ArgException &e)
    {
        Leosac::print_exception(e);
        return 1;
    }

    if (set_working_directory(options) != 0)
        return 1;

    while (relaunch)
    {
        UnixShellScript backup("cp -f");

        try
        {
            Kernel kernel(Kernel::make_config(options), options.is_strict());
            relaunch = kernel.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception propagated to main(). Will now exit." << std::endl;
            Leosac::print_exception(e);
            return 1;
        }
        catch (...)
        {
            std::cerr << "Unkown exception in main" << std::endl;
            return 1;
        }
    }
    INFO("Bye.");
    return (0);
}
