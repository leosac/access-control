Leosac Documentation Mainpage {#mainpage}
=========================================

Introduction {#main_intro}
==========================

See [modules](@ref modules) for a bit of info about modules.

We recommend you read through the related pages informations, there you will find documentation for 
each module and how to configure and use them.

Interesting stuff:
   + See [FGPIO](@ref Leosac::Hardware::FGPIO) detailed description for GPIO command.
   + See [FLED](@ref Leosac::Hardware::FLED) detailed description for LED command.
   + [General configuration information](@ref general_config_main) (logger / network / remote control).
   + Understand the [authentication system](@ref auth_main)
   + [Firmware](@ref firmware_main) info
   + [Path management](@ref general_config_path_mng)
   + Overview of the [Remote Control](@ref remote_control_main) interface.
   
You can find an [install guide](@ref install_guide) for a simple setup: Raspberry Pi + Piface + Wiegand + Door. 

Usage {#usage}
==============

`./leosac [-d <working_directory>] -k <config_file> [-v] [--] [--version] [-h]`

Short | Long                   | Description
------|------------------------|------------
-d    | `--working-directory`  | Directory to cd to.
-k    | `--kernel-cfg`         | Leosac's core configuration file
-h    | `--help`               | Print the help
N/A   | `--version`            | Display version informations and exit

Notes
=====

`boost::serialization` version shall be the same on all your Leosac unit.
This will prevent trouble when passing around configuration.
