#!/bin/bash

set -e
set -x

cd /tmp

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

# MySQL runtime
wget http://www.codesynthesis.com/download/odb/2.4/libodb-mysql-2.4.0.tar.gz && tar xvf libodb-mysql-2.4.0.tar.gz
cd libodb-mysql-2.4.0 && ./configure && make -j4 && make install

#PGSQL runtime
wget http://www.codesynthesis.com/download/odb/2.4/libodb-pgsql-2.4.0.tar.gz && tar xvf libodb-pgsql-2.4.0.tar.gz
cd libodb-pgsql-2.4.0 && ./configure && make -j4 && make install
