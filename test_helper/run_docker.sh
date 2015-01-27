#!/bin/bash
#
# Run the integration test in docker container to provide isolation
#
# $1 is test name
#

set -x
set -e

RCol='\e[0m' # Text Reset 
Red='\e[0;31m';

## Name of the leosac image.
DOCKER_CONTAINER=leosac

docker run --privileged=true -t ${DOCKER_CONTAINER} "(pushd /leosac_src/test_helper/$1/ && INSTALL_DIR=/usr/local ./run_test.sh && popd)" \
    || { echo  -e ${Red}"Test $1 failed :(" ${RCol} ; exit 1; }

exit 0
