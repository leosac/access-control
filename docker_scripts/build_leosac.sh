#!/bin/bash
#
# This script will build leosac (assuming source are at /leosac_src)
# and will install into /usr/local

set -x
set -e

mkdir -p /tmp/leosac_build;
pushd /tmp/leosac_build;

(cmake -DCMAKE_BUILD_TYPE=Debug -DLEOSAC_BUILD_TESTS=1 -DZMQ_BUILD_TESTS=off -DZMQPP_LIBZMQ_CMAKE=0 /leosac_src && make -j5 && make install)

popd;
