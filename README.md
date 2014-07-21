# Leosac

Open Source Physical Access Control System

## Install

In progress

## Build from source

You will need a working Linux environment and the following packages:
* GCC 4.8+ (any C++11-compatible compiler should do)
* CMake 2.8
* Boost 1.41 minimum
* TCLAP

CMake takes care of the build process, here's a minimal procedure to compile Leosac:
```shell
cd path/to/leosac
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Advanced CMake flags

These flags are mainly aimed for developers, and provide flexibility to the build process.

* `LEOSAC_PLATFORM`

Leosac is mainly targeted for Linux ARM Boards, so you'll want to inform CMake if you're using a specific board. This is done by setting the `LEOSAC_PLATFORM` var when configuring CMake. If the platform is recognized, some features or optimisations may be enabled under-the-hood. Setting an invalid name will not break anything.

Available platforms (case insensitive):
* RaspberryPi
* CubieBoard

Example:  `cmake .. -DLEOSAC_PLATFORM=Cubieboard`

* `LEOSAC_GPROF`

CMake will add Gprof compilation/linking flags. This flag is for debugging purpose only and should be coupled with `-DCMAKE_BUILD_TYPE=Debug`

* `LEOSAC_BUILD_MODULES`

Unset this flag if you don't want CMake to build Leosac modules.

## License

This project is released under the Affero GPLv3 license.
A copy of the license can be found in the root folder.

![alt text][logo]
[logo]: https://raw.githubusercontent.com/islog/leosac/master/rc/AGPLv3.png  "AGPL Logo"
