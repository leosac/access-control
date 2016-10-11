#!/bin/bash

cd /usr/src/gtest
cmake .
make
cp libgtest* /usr/lib/
