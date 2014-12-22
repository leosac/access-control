# Leosac [![Build Status](https://travis-ci.org/islog/leosac.png?branch=develop)](https://travis-ci.org/islog/leosac)

## What is Leosac?

Leosac is an open source physical access control system software. It is meant to be installed on embedded devices running Linux, to handle the logic processing of an access control. Basically, it opens doors.

A Leosac setup is usually composed of an embedded Linux board running the software, locked in a case, connected to a RFID card reader and to the electric strike of a door. 

## Features

* Multiple-door/reader support
* Ethernet connectivity
* **Fully** configurable wiring layout
* Portable on virtually every Linux devices on the market
* Open-source

## Getting started

If you are new to Leosac and want to build your first setup, please follow this [basic setup guide](https://github.com/islog/leosac/wiki/Basic-Setup) to get you started.

## Usage

```shell
leosac  [-g <string>] [-f <string>] [-v] [--] [--version] [-h]`
```

| Flag | Description          |
| ------------- | ----------- |
| `-k`, `--kernel-cfg` | Core configuration file path ([more](https://github.com/islog/leosac/wiki/Configuration)) |
| `-v`, `--verbose` | Increase verbosity |
| `--version` | Displays version information and exits. |
| `-h`, `--help` | Display usage information |

## Build and install

See the [wiki](https://github.com/islog/leosac/wiki/Build-and-install)

## Issues

Please report issues [here](https://github.com/islog/leosac/issues).

## License

This project is released under the GNU Affero GPLv3 license.
A copy of the license can be found in the root folder.

![alt text][logo]
[logo]: https://raw.githubusercontent.com/islog/leosac/master/rc/AGPLv3.png  "AGPL Logo"
