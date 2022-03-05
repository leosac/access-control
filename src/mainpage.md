@mainpage Leosac Manual

[TOC]

[![leosac.com](logo_banner.png)](http://www.leosac.com)

Introduction {#main_intro}
==========================

What is Leosac? {#what_is_leosac}
---------------------------------

Leosac is an open source physical access control system software.
It is meant to be installed on embedded devices running Linux, to handle the logic processing of an access control.
Basically, it opens doors.

A Leosac setup is usually composed of an embedded Linux board running the software,
locked in a case, connected to a RFID card reader and to the electric strike of a door.

Features {#leosac_features}
---------------------------

* Multiple-door/reader support
* Ethernet connectivity
* Fully configurable wiring layout
* Portable on virtually every Linux devices on the market
* Open-source

Documentation summary {#doc_summary}
------------------------------------

See [modules](@ref modules) for a bit of info about modules.

We recommend you read through the [related pages](pages.html) informations, there you will find documentation for
each module and how to configure and use them.

Interesting stuff (for end user):
   + [General configuration information](@ref page_general_config) (logger / network / remote control).
   + [Install guide](@ref install_guide) for a simple setup: Raspberry Pi + Piface + Wiegand + Door.

Interesting stuff (for devs):
   + See [FGPIO](@ref Leosac::Hardware::FGPIO) detailed description for GPIO command.
   + See [FLED](@ref Leosac::Hardware::FLED) detailed description for LED command.
   + Understand the [authentication system](@ref auth_main)
   + [Firmware](@ref firmware_main) info
   + [Path management](@ref general_config_path_mng)
   + Overview of the [Remote Control](@ref remote_control_main) interface.

Usage {#usage}
--------------


`./leosac [-d <working_directory>] -k <config_file> [-s] [-v] [--] [--version] [-h]`

Short | Long                   | Description
------|------------------------|------------
-d    | `--working-directory`  | Directory to cd to.
-k    | `--kernel-cfg`         | Leosac's core configuration file
-h    | `--help`               | Print the help
-s    | `--strict`             | Run more deadly check against the configuration (recommended)
N/A   | `--version`            | Display version informations and exit

Notes {#notes}
--------------

`boost::serialization` version shall be the same on all your Leosac unit.
This is to prevent problem when synchronizing configuration between Leosac unit.

Issues {#issues}
----------------

Please report issues (and ask questions) [here](https://github.com/leosac/leosac/issues).

License {#license}
------------------

This project is released under the GNU Affero GPLv3 license.
A copy of the license can be found in the root folder.

![alt text][logo]
[logo]: https://raw.githubusercontent.com/leosac/leosac/master/rc/AGPLv3.png  "AGPL Logo"


Subpages:
  * @subpage page_doc_content
