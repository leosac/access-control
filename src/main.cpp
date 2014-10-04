/**
 * \file main.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief standard main
 */

#include <iostream>

// NOTE TCLAP is easily replacable by boost::program_options
#include <tclap/CmdLine.h>
#include <core/kernel.hpp>
#include <exception/configexception.hpp>

#include "tools/log.hpp"
#include "tools/leosac.hpp"
#include "tools/runtimeoptions.hpp"
#include "tools/unixfs.hpp"
#include "tools/unixshellscript.hpp"
#include "core/core.hpp"
#include "exception/leosacexception.hpp"

static const std::string  DefaultCoreCfg = UnixFs::getCWD() + "/cfg/default.xml";
static const std::string  DefaultHwCfg = UnixFs::getCWD() + "/cfg/hardware.xml";

int main(int argc, char** argv)
{
    RuntimeOptions  options;
    int             relaunch = 1;

    try {
        try {
            TCLAP::CmdLine                  cmd("Open Source Access Controller", ' ', Leosac::getVersionString());
            TCLAP::SwitchArg                verboseSwitch("v", "verbose", "Increase verbosity", false);
            TCLAP::SwitchArg                testSwitch("t", "test-run", "Test run usable by valgrind", false);
            TCLAP::ValueArg<std::string>    coreFile("f", "core-cfg", "Configuration file", false, DefaultCoreCfg, "string");
            TCLAP::ValueArg<std::string>    kernelFile("k", "kernel-cfg", "Kernel Configuration file", false, "", "string");
            TCLAP::ValueArg<std::string>    hwFile("g", "hw-cfg", "Hardware configuration file", false, DefaultHwCfg, "string");

            cmd.add(verboseSwitch);
            cmd.add(testSwitch);
            cmd.add(coreFile);
            cmd.add(hwFile);
            cmd.add(kernelFile);
            cmd.parse(argc, argv);
            options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
            options.setFlag(RuntimeOptions::TestRun, testSwitch.getValue());
            options.setParam("corecfg", coreFile.getValue());
            options.setParam("hwcfg", hwFile.getValue());
            options.setParam("kernel-cfg", kernelFile.getValue());
        }
        catch (const TCLAP::ArgException &e) {
            throw (LEOSACException(e.error()));
        }
        while (relaunch)
        {
            UnixShellScript backup("cp -f");

            //backup.run(UnixShellScript::toCmdLine(options.getParam("corecfg"), "core.cfg"));
            //backup.run(UnixShellScript::toCmdLine(options.getParam("hwcfg"), "hw.cfg"));

            Kernel kernel(Kernel::make_config(options));

            relaunch = false;
            relaunch = kernel.run();
           /* Core    core(options);
            relaunch = core.run();

            // Yet an other hack. dhclient "steals" the bound port of Rpleth and prevent the program
            // to restart.
            UnixShellScript kill("killall dhclient");
            kill.run();*/
           // std::this_thread::sleep_for(std::chrono::milliseconds(7000));
        }
    }
    catch (const LEOSACException& e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
