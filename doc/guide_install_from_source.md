@page page_guid_install_from_source Building Leosac

@brief Describe how to build Leosac from source.


Target platform
---------------

This guide target a Debian Stretch system running an amd64 platform.

Dependencies
------------


Leosac as a number of dependencies that needs to be installed:
```
apt-get install cmake build-essential git         \
libssl-dev                                        \
libcurl4-openssl-dev libtclap-dev libscrypt-dev   \
libzmq3-dev                                       \
python3 python3-pip
```

Database stuff

First get the ODB compiler package from http://www.codesynthesis.com/products/odb/download.xhtml.
Direct link: http://www.codesynthesis.com/download/odb/2.4/odb_2.4.0-1_amd64.deb

@note The ODB package from the Debian repository is broken.

```
apt-get install libsqlite3-dev libpq-dev \
libodb-dev libodb-boost-dev libodb-sqlite-dev libodb-pgsql-dev
```

Boost libraries (todo)

```
apt-get install libboost1.62-all-dev
```


Build
-----

Clone the repo: `git clone https://github.com/leosac/leosac.git`
Now go to the `leosac` directory.

```
git submodule init && git submodule update
mkdir build;
cd build;
cmake ..
make -j10
```
