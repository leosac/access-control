# Leosac [![Build Status](https://travis-ci.org/islog/leosac.png?branch=develop)](https://travis-ci.org/islog/leosac)

Open Source Physical Access Control System

## Usage

In progress...

## Build from source

You will need a working Linux environment and the following packages:
* GCC 4.8+ (any C++11-compatible compiler should do)
* CMake 2.8 (and above)
* Boost 1.41 (and above)
* TCLAP

CMake takes care of the build process, here's a minimal procedure to compile Leosac:
```shell
cd path/to/leosac
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DLEOSAC_PLATFORM=None -DCMAKE_INSTALL_PREFIX=/usr/local
make
```
The `LEOSAC_PLATFORM` var is set to 'None' by default, you can safely ignore it for now, or see **Advanced CMake flags** section for more info.

## Install

Pre-compiled packages are not available for now. You will have to follow instruction from the **Build from source** section first.
You can change the install path specified in `-DCMAKE_INSTALL_PREFIX=/usr/local`, and then execute:
```shell
make install
```
If you have insufficient rights on the install directory, execute as root:
```shell
sudo make install
```

### Advanced CMake flags

These flags are mainly aimed for developers, and provide flexibility to the build process.

* `LEOSAC_PLATFORM`

Leosac is mainly targeted for Linux ARM Boards, so you'll want to inform CMake if you're using a specific board. This is done by setting the `LEOSAC_PLATFORM` var when configuring CMake. If the platform is recognized, some features or optimisations may be enabled under-the-hood. Setting an invalid name will not break anything.

Available platforms (case insensitive):
* None (Default value)
* RaspberryPi
* CubieBoard

Example:  `cmake .. -DLEOSAC_PLATFORM=Cubieboard`

* `LEOSAC_GPROF`

CMake will add [GNU Gprof](http://en.wikipedia.org/wiki/Gprof) compilation/linking flags. This flag is for debugging purpose only and should be coupled with `-DCMAKE_BUILD_TYPE=Debug`

* `LEOSAC_BUILD_MODULES`

Unset this flag if you don't want CMake to build Leosac modules.

## License

This project is released under the Affero GPLv3 license.
A copy of the license can be found in the root folder.

![alt text][logo]
[logo]: https://raw.githubusercontent.com/islog/leosac/master/rc/AGPLv3.png  "AGPL Logo"
