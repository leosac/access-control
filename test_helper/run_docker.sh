#!/bin/bash
#
# Run the integration test in docker container to provide isolation
#
# $1 is test name
#

set -x
set -e

## Name of the leosac image.
DOCKER_CONTAINER=leosac

docker run -t ${DOCKER_CONTAINER} "(pushd /leosac_src/test_helper/$1/ && INSTALL_DIR=/usr/local ./run_test.sh && popd)" \
 || echo "Test $1 FAILED"

exit 0
