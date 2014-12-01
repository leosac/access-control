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

static int set_working_directory(RuntimeOptions &opts)
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

int main(int argc, const char **argv)
{
    RuntimeOptions options;
    int relaunch = 1;

    try
    {
        TCLAP::CmdLine cmd("Open Source Access Controller", ' ', Leosac::getVersionString());
        TCLAP::SwitchArg verboseSwitch("v", "verbose", "Increase verbosity", false);
        TCLAP::ValueArg<std::string> kernelFile("k", "kernel-cfg", "Kernel Configuration file", true, "", "string");
        TCLAP::ValueArg<std::string> working_directory("d", "working_directory", "Leosac's working directory", false, "", "string");

        cmd.add(verboseSwitch);
        cmd.add(kernelFile);
        cmd.add(working_directory);
        cmd.parse(argc, argv);
        options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
        options.setParam("kernel-cfg", kernelFile.getValue());
        options.setParam("working_directory", working_directory.getValue());
    }
    catch (const TCLAP::ArgException &e)
    {
        throw (LEOSACException(e.error()));
    }

    if (set_working_directory(options) != 0)
        return 1;

    while (relaunch)
    {
        UnixShellScript backup("cp -f");

        try
        {
            Kernel kernel(Kernel::make_config(options));
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
