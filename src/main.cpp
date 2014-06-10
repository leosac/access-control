/**
 * \file main.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief standard main
 */

#include <iostream>

// NOTE TCLAP is easily replacable by boost::program_options
#include <tclap/CmdLine.h>

#include "osac.hpp"
#include "tools/log.hpp"
#include "tools/runtimeoptions.hpp"
#include "tools/unixfs.hpp"
#include "core/core.hpp"
#include "exception/osacexception.hpp"

static const std::string  DefaultCoreCfg = UnixFs::getCWD() + "/cfg/default.xml";
static const std::string  DefaultHwCfg = UnixFs::getCWD() + "/cfg/hardware.xml";

int main(int argc, char** argv)
{
    RuntimeOptions  options;

    try {
        try {
            TCLAP::CmdLine                  cmd("Open Source Access Controller", ' ', OSAC::getVersionString());
            TCLAP::SwitchArg                verboseSwitch("v", "verbose", "Increase verbosity", false);
            TCLAP::ValueArg<std::string>    coreFile("f", "core-cfg", "Configuration file", false, DefaultCoreCfg, "string");
            TCLAP::ValueArg<std::string>    hwFile("g", "hw-cfg", "Hardware configuration file", false, DefaultHwCfg, "string");

            cmd.add(verboseSwitch);
            cmd.add(coreFile);
            cmd.add(hwFile);
            cmd.parse(argc, argv);
            options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
            options.setParam("corecfg", coreFile.getValue());
            options.setParam("hwcfg", hwFile.getValue());
        }
        catch (const TCLAP::ArgException &e) {
            throw (OSACException(e.error()));
        }
        Core    core(options);

        core.run();
    }
    catch (const OSACException& e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
