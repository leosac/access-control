@page page_guide_install_from_source Building Leosac

@brief Describe how to build Leosac from source.


Background
----------

This guide is intended to be used on Debian Stretch with amd64 architecture or Raspbian Stretch with armv6/armv7 architecture.
While not quite as slow as building Leosac into a deb package, this method can still takes hours to complete when building natively on a Raspberry Pi.


Dependencies
------------

@note The ODB package from the Debian & Raspbian repositories is broken. A bug report has been filed. See [889664](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=889664). Until this is resolved, a set of patched odb packages have been made available in the Leosac [bin-resources](https://github.com/leosac/bin-resources) repository.

The following steps assume the odb package is not already installed on the target system. If it is, uninstall it first.
  + `git clone https://github.com/leosac/bin-resources`
  + `cd bin-resources/debian/gcc6/armhf`
  + `sudo dpkg -i odb_2.4.0*.deb`
  + `sudo apt-mark hold odb`
  + `sudo apt-get install -f`

If running Raspbian, replace amd64 in the folder name shown above with armhf.

Leosac has a number of additonal dependencies which need to be installed:
```
sudo apt-get install cmake build-essential pkg-config git \
default-libmysqlclient-dev libtclap-dev libcurl4-openssl-dev libgtest-dev \
libunwind-dev libzmq3-dev libpq-dev libpython2.7-dev libscrypt-dev \
libsqlite3-dev libsodium-dev libssl-dev libboost-date-time-dev \
libboost-filesystem-dev libboost-regex-dev libboost-serialization-dev \
libboost-system-dev python3 python3-pip libodb-boost-dev libodb-mysql-dev \
libodb-pgsql-dev libodb-sqlite-dev libodb-dev
```


Build
-----

Clone the repo and peform a legacy make install from-source:
  + `git clone https://github.com/leosac/leosac.git`
  + `cd leosac`
  + `git submodule init && git submodule update`
  + `mkdir build`
  + `cd build`
  + `cmake -DCMAKE_BUILD_TYPE=Release ..`
  + `make`
  + `sudo make install`


Follow-Up Tasks
---------------

Leosac is now installed on your system, but there are couple of additional tasks you should perform.

Edit the leosac service file, changing `/usr/bin/leosac` to `/usr/local/bin/leosac`, then copy it into place:
  + `vi/gedit ../pkg/debian/leosac.service`
  + `sudo cp ../pkg/debian/leosac.service /etc/systemd/system`

@note The steps above assume you are still in the build subfolder created earlier.

Finally, before you can start Leosac, you need to create a kernel.xml file. See the [installation guide](@ref page_guide_rpi_piface_wiegand).

