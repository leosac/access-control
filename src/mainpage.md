Leosac Documentation Mainpage {#mainpage}
=========================================

Introduction {#main_intro}
=========================

See [modules](@ref modules) for an overview of available modules.

Interesting stuff:
   + See FGPIO detailed description for GPIO command.
   + See FLED detailed description for LED command.
   + Configure the [logger](@ref logger_main)
   + Configure the [network](@ref network_main)
   + Understand the [authentication system](@ref auth_main)
   + [Firmware](@ref firmware_main) info
   + [Path management](@ref path_management)
   
You can find an [install guide](@ref install_guide) for a simple setup: Raspberry Pi + Piface + Wiegand + Door. 

Usage {#usage}
==============

`./leosac [-d <working_directory>] -k <config_file> [-v] [--] [--version] [-h]`

Short | Long                   | Description
------|------------------------|------------
-d    | --working_directory    | Directory to cd to.
-k    | --kernel-cfg           | Leosac's core configuration file
-h    | --help                 | Print the help
      | --version              | Display version informations and exit