/**
 * \file main.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief standard main
 */

#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>

#include "osac.hpp"
#include "core/core.hpp"
#include "exception/osacexception.hpp"
#include "runtimeoptions.hpp"

int main(int argc, char** argv)
{
    try
    {
        RuntimeOptions                  options;
        TCLAP::CmdLine                  cmd("Open Source Access Controller", ' ', OSAC::getVersionString());
        TCLAP::ValueArg<std::string>    nameArg("n","name","Name to print",true,"homer","string");
        TCLAP::SwitchArg                verboseSwitch("v", "verbose", "Increase verbosity", false);

//         cmd.add(nameArg);
        cmd.add(verboseSwitch);
        cmd.parse(argc, argv);

//         std::string name = nameArg.getValue();
        options.setFlag(RuntimeOptions::Verbose, verboseSwitch.getValue());
    }
    catch (const TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    try
    {
        Core    core;

        core.run();
    }
    catch (const OSACException& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
