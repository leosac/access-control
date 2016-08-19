#!/bin/bash
#
# This script will build leosac (assuming source are at /leosac_src)
# and will install into /usr/local

set -x
set -e

mkdir -p /tmp/leosac_build;
pushd /tmp/leosac_build;

(cmake /leosac_src && (make -j5 || make -j5 || make -j5 || make || make) && make install)

popd;
