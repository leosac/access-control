#!/bin/bash
#
# Run shell script based tests
#

for d in `find . -type d`; do
    if [ $d = "." ] || [ $d = ".." ] ; then continue; fi
    echo "Will run test: $d"
    pushd $d
    [ -x run_test.sh ] || { echo "Not run_test.sh files in subdirectory"; exit 1; }
    ./run_test.sh || { echo "Test failed"; exit 1; }
    popd
done

exit 0
