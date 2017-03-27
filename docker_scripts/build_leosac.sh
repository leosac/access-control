#!/bin/bash
#
# This script will build leosac (assuming source are at /leosac_src)
# and will install into /usr/local

set -x
set -e

mkdir -p /tmp/leosac_build;
pushd /tmp/leosac_build;

# We disable Stacktrace cause its a big memory leak
# right now.
(cmake -DCMAKE_BUILD_TYPE=Debug -DLEOSAC_BUILD_TESTS=1 -DZMQ_BUILD_TESTS=off \
       -DLEOSAC_STACKTRACE_DISABLED=2 \
       -DZMQPP_LIBZMQ_CMAKE=0 /leosac_src && make -j5 && make install)

popd;
