@page page_guide_install_from_package Build and install Debian package

@brief Describe how to build and install a Leosac deb package.

Target platform
---------------

This guide is intended to be used on Debian Stretch with amd64 architecture or Raspbian Stretch with armv6/armv7 architecture.

Dependencies
------------

For starters, install these packages:
```
apt-get install cmake build-essential git sudo devscripts
```

There is a bug in the ODB compiler found in the Debian and Raspbian repos, which prevents Leosac from building.
A bug report has been filed. See [#889664](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=889664).
Until this has been resolved and new pacakges are pushed out to the Debian and Raspbian repos, one must use the odb packages in the Leosac bin-resources repo instead.

The following steps were performed on a Raspberry PI running Stretch with gcc 6.4 installed.
The folder you choose may differ from this example. Choose the appropriate subfolder names which match your architecture and version of gcc.
```
git clone https://github.com/leosac/bin-resources
cd bin-resources/debian/gcc6.4/armhf
sudo dpkg -i *.deb
```

There are additional dependencies required to build Leosac, but those will be installed for you by the deb.sh script.

Build
-----

Get the Leosac Debian packaging script and mark it executable:
```
wget https://raw.githubusercontent.com/leosac/leosac/develop/deb.sh
chmod +x deb.sh
```

If you intend to build the very latest code from the develop branch, then simply call deb.sh with no parameters to start the build process:
```
./deb.sh
```

If instead you prefer to build the latest release (or some other branch), then use the -b flag to specify the branch, tag, or release to build:
```
./deb.sh -b v0.7.0
```

Replace "v0.70" with the tag name of the most recent release, found on the leosac releases page.

That's it!
When the build completes, it will present the name of the folder, under /tmp, where the newly built package files reside.
From the command line, navigate to that folder and install the Leosac debian package file using dpkg -i or gdebi.

