@page page_guide_install_from_source Building Leosac

@brief Describe how to build Leosac from source.


Background
----------

This guide is intended to be used on Debian Bullseye with amd64 architecture or Raspbian Stretch with armv6/armv7/arm64 architecture.
While not quite as slow as building Leosac into a deb package, this method can still takes hours to complete when building natively on a Raspberry Pi.


Dependencies
------------

You will need a working Linux environment and the following packages:

 * GCC 4.8+ (any C++11-compatible compiler should do)
 * Git (to clone the repo)
 * CMake 2.8.8 (and above)
 * Boost 1.41 (and above)
 * ODB 2.4 (and above)
 * TCLAP
 * GoogleTest is required if you plan to build the test suite.

Such dependencies need to be installed:
```
sudo apt-get install cmake build-essential pkg-config git apt-utils libssl-dev libzmq5 libzmq3-dev libcurl4-openssl-dev libtclap-dev libscrypt-dev python3 python3-pip gcc-9 g++-9 libboost-all-dev libgtest-dev libsqlite3-dev libpq-dev odb libodb-dev libodb-pgsql-dev libodb-pgsql-2.4 libodb-sqlite-2.4 libodb-sqlite-dev libodb-boost-dev
```


Build
-----

*Note:* Make sure `g++ --version` shows at least 4.8. You can use the `update-alternative` command on Debian to set the default version of `gcc/g++`.

Clone the repo and peform a legacy make install from-source:
```
git clone -b master https://github.com/islog/leosac.git
cd leosac
git submodule init
git submodule update
```

`libzmq` and `zmqpp` are bundled with the project (using git submodules).

CMake takes care of the build process, here's a minimal procedure to compile Leosac:
```
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DZMQPP_BUILD_STATIC=0 -DZEROMQ_LIB_DIR=`pwd`/libzmq/.libs/ -DZEROMQ_INCLUDE_DIR=`pwd`/../libzmq/include -DZMQPP_LIBZMQ_CMAKE=1
make
```

If you have difficulties building libzmq, install it from debian repository instead.
```
apt install libzmq5 libzmq3-dev
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DZMQPP_BUILD_STATIC=0 -DZMQPP_LIBZMQ_CMAKE=0
make
```

    * `LEOSAC_GPROF`: CMake will add [GNU Gprof](https://en.wikipedia.org/wiki/Gprof) compilation/linking flags. This flag is for debugging purpose only and should be coupled with `-DCMAKE_BUILD_TYPE=Debug`
    * `LEOSAC_BUILD_MODULES`: Unset this flag if you don't want CMake to build Leosac modules.

Follow-Up Tasks
---------------

Leosac is now installed on your system, but there are couple of additional tasks you should perform.

Edit the leosac service file, changing `/usr/bin/leosac` to `/usr/local/bin/leosac`, then copy it into place:
  + `vi/gedit ../pkg/debian/leosac.service`
  + `sudo cp ../pkg/debian/leosac.service /etc/systemd/system`

@note The steps above assume you are still in the build subfolder created earlier.

Finally, before you can start Leosac, you need to create a kernel.xml file. See the [installation guide](@ref page_guide_rpi_piface_wiegand).

Distcc
------

[distcc](https://www.distcc.org/) is a powerful tool that allows cross-compilation in a distributed manner. It is strongly recommended to setup distcc when developing leosac if you're building directly on raspberry-like hardware because standard ARM boards are very slow in the compilation stage.

A proper setup with a standard x86/x64 machine can easily speed up the process by a factor of ten.
