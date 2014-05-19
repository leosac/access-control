/**
 * \file main.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief standard main
 */

#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>

#include "osac.hpp"
#include "tools/unixfs.hpp"
#include "core/core.hpp"
#include "exception/osacexception.hpp"
#include "runtimeoptions.hpp"

static const std::string  DefaultConfigFile = UnixFs::getCWD() + "/cfg/default.xml";

int main(int argc, char** argv)
{
    RuntimeOptions  options;

    try {
        try {
            TCLAP::CmdLine                  cmd("Open Source Access Controller", ' ', OSAC::getVersionString());
            TCLAP::SwitchArg                verboseSwitch("v", "verbose", "Increase verbosity", false);
            TCLAP::ValueArg<std::string>    configFile("f", "config-file", "Configuration file", false, DefaultConfigFile, "string");

            cmd.add(verboseSwitch);
            cmd.add(configFile);
            cmd.parse(argc, argv);
            options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
            options.setParam("configfile", configFile.getValue());
        }
        catch (const TCLAP::ArgException &e) {
            std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
        }
        Core    core(options);

        core.run();
    }
    catch (const OSACException& e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
