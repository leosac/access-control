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

docker run -t leosac "(pushd /leosac_src/test_helper/$1/ && INSTALL_DIR=/usr/local ./run_test.sh && popd)"

exit 0
