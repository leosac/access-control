@page page_guide_install_from_source Building Leosac

@brief Describe how to build Leosac from source.


Target platform
---------------

This guide is intended to be used on Debian Stretch with amd64 architecture or Raspbian Stretch with armv6/armv7 architecture.

Dependencies
------------

Leosac has a number of dependencies that need to be installed:
```
sudo apt-get install cmake build-essential git \
default-libmysqlclient-dev libtclap-dev libcurl4-openssl-dev libgtest-dev \
libunwind-dev libzmq3-dev libpq-dev libpython2.7-dev libscrypt-dev \
libsqlite3-dev libsodium-dev libssl-dev libboost-date-time-dev \
libboost-filesystem-dev libboost-regex-dev libboost-serialization-dev \
libboost-system-dev python3 python3-pip
```

@note The ODB package from the Debian repository is broken. A bug report has been filed. See [889664](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=889664). Until this is resolved, a set of patched odb packages have been made available in the Leosac bin-resources repository

The following steps assume there are not already any odb packages installed on the target system. If there are, uninstall them first.
```
git clone https://github.com/leosac/bin-resources
cd bin-resources/debian/gcc6/amd64
sudo dpkg -i *.deb
sudo apt-get install -f
```
If running Raspbian, replace amd64 in the folder name shown above with armhf.

Build
-----

Clone the repo: `git clone https://github.com/leosac/leosac.git`
Now go to the `leosac` directory.

```
git submodule init && git submodule update
mkdir build;
cd build;
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```

Follow-Up Tasks
---------------

Leosac is now installed on your system, but there are couple of tasks you should perform.

Copy the init script into place so that Leosac can be started as a service:
```
sed -i 's/\/usr\/bin\/leosac/\/usr\/local\/bin\/leosac/g' ../pkg/deb/leosacd
sudo install -m 755 -t "/etc/init.d" "../pkg/deb/leosacd"
```
The steps above assume you are still in the build subfolder created earlier.

Create a kernel.xml file (see the [installation guide](@ref page_guide_rpi_piface_wiegand)) and copy it to /etc/leosac.d.
```
sudo install -m 755 -d "/etc/leosac.d"
```
