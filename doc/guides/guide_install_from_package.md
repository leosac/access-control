@page page_guide_install_from_package Build and install Debian package

@brief Describe how to build and install a Leosac deb package.


Background
----------

This guide is intended to be used on Debian Stretch with amd64 architecture or Raspbian Stretch with armv6/armv7 architecture.
The process desibed here is a straightforward and relatively simple approach to building and installing Leosac on the native hardware.
It does this at the expense of speed. The build process described here will take several hours to complete on a modern Raspberry Pi, and will take around 12 hours or more to complete on the model 1 Pi's and the Zero.
Consider using one of the other build methods if build time is important.


Dependencies
------------

For starters, install these packages:
  + `apt-get install cmake build-essential pkg-config git sudo devscripts`

@note The ODB package in the Debian & Raspbian repositories is broken. A bug report has been filed. See [889664](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=889664). Until this is resolved, a set of patched odb packages have been made available in the Leosac [bin-resources](https://github.com/leosac/bin-resources) repository.

The following steps assume the odb package is not already installed on the target system. If it is, uninstall it first.
  + `git clone https://github.com/leosac/bin-resources`
  + `cd bin-resources/debian/gcc6/amd64`
  + `sudo dpkg -i odb_2.4.0*.deb`
  + `sudo apt-mark hold odb`
  + `sudo apt-get install -f`

If running Raspbian, replace amd64 in the folder name shown above with armhf.
There are additional dependencies required to build Leosac, but those will be installed for you by the deb.sh script.


Build
-----

Get the Leosac Debian packaging script and mark it executable:
  + `wget https://raw.githubusercontent.com/leosac/leosac/develop/deb.sh`
  + `chmod +x deb.sh`

If you intend to build the very latest code from the develop branch, then simply call deb.sh with no parameters to start the build process:
  + `./deb.sh`

If instead you prefer to build the latest release (or some other branch), then use the -b flag to specify the branch, tag, or release to build:
  + `./deb.sh -b v0.7.0`

Replace "v0.70" with the tag name of the most recent release, found on the leosac [releases page](https://github.com/leosac/leosac/releases).

Let's say you have development work in a branch in your own fork you want to test, you can call deb.sh this way:
  + `./deb.sh -u https://github.com/your-git-profile/leosac -b your-branch` 

When the build completes, it will present the name of any arbitrary folder under /tmp where the newly built package files reside.
From the command line, navigate to that folder and install the Leosac debian package file using dpkg -i or gdebi.
  + `cd /tmp/some-random-name`
  + `sudo gdebi leosac_0.7.0-1_amd64.deb`

Leosac is now installed, but before you can start it, you need to create a kernel.xml file. See the [installation guide](@ref page_guide_rpi_piface_wiegand).

