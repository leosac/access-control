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
* \file main.cpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief standard main
*/

#include "core/kernel.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/leosac.hpp"
#include "tools/log.hpp"
#include "tools/unixshellscript.hpp"
#include <iostream>
#include <tclap/CmdLine.h> // Could be replaced by boost::program_options
#include <unistd.h>

using namespace Leosac::Tools;
using namespace Leosac;

static int set_working_directory(RuntimeOptions &opts) noexcept
{
    int ret = 0;
    if (opts.has_param("working_directory") &&
        !opts.get_param("working_directory").empty())
    {
        ret = chdir(opts.get_param("working_directory").c_str());
        if (ret != 0)
            perror("Cannot change working directory");
    }
    return ret;
}

int main(int argc, const char **argv)
{
    RuntimeOptions options;
    int relaunch = 1;

    try
    {
        TCLAP::CmdLine cmd("Open Source Access Controller", ' ',
                           Leosac::getVersionString());
        TCLAP::SwitchArg strict("s", "strict",
                                "Be strict regarding configuration error",
                                false); // assert on configuration error.
        TCLAP::ValueArg<std::string> kernelFile(
            "k", "kernel-cfg", "Kernel Configuration file", true, "", "config_file");
        TCLAP::ValueArg<std::string> working_directory(
            "d", "working-directory", "Leosac's working directory", false, "",
            "working_directory");

        cmd.add(strict);
        cmd.add(kernelFile);
        cmd.add(working_directory);
        cmd.parse(argc, argv);
        options.set_param("kernel-cfg", kernelFile.getValue());
        options.set_param("working_directory", working_directory.getValue());
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
        try
        {
            INFO("Creating Leosac Kernel...");
            Kernel kernel(Kernel::make_config(options), options.is_strict());
            relaunch = kernel.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception propagated to main(). Will now exit."
                      << std::endl;
            Leosac::print_exception(e);
            return 1;
        }
        catch (...)
        {
            std::cerr << "Unknown exception in main" << std::endl;
            return 1;
        }
    }
    INFO("Bye.");
    return (0);
}
