#!/bin/bash

set -e
set -x

cd /tmp

# Current packages on Debian Bullseye and Ubuntu 20.04.4 have
# been tested successfully. Manual build of ODB shouldn't be
# required on these systems.

# ODB 2.4 do not build properly with G++9 nd ODB 2.5 which will
# officially support latest G++ versions is not released yet.
# Manually building ODB may also be impacted on latest Debian by
# https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=980609

wget http://www.codesynthesis.com/download/odb/2.4/odb_2.4.0-1_amd64.deb && dpkg -i odb_2.4.0-1_amd64.deb

# Common runtime
wget http://www.codesynthesis.com/download/odb/2.4/libodb-2.4.0.tar.gz && tar xvf libodb-2.4.0.tar.gz
cd libodb-2.4.0 && ./configure && make -j4 && make install

echo "Will now install ODB SQLite"

# SQLite runtime
wget http://www.codesynthesis.com/download/odb/2.4/libodb-sqlite-2.4.0.tar.gz && tar xvf libodb-sqlite-2.4.0.tar.gz
cd libodb-sqlite-2.4.0 && ./configure && make -j4 && make install

wget http://www.codesynthesis.com/download/odb/2.4/libodb-boost-2.4.0.tar.gz && tar xvf libodb-boost-2.4.0.tar.gz
cd libodb-boost-2.4.0 && ./configure && make -j4 && make install

#PGSQL runtime
wget http://www.codesynthesis.com/download/odb/2.4/libodb-pgsql-2.4.0.tar.gz && tar xvf libodb-pgsql-2.4.0.tar.gz
cd libodb-pgsql-2.4.0 && ./configure && make -j4 && make install
