#!/bin/bash
#
# Run shell script based tests
#

RCol='\e[0m' # Text Reset
Bla='\e[0;30m';
Red='\e[0;31m';
Gre='\e[0;32m';
Yel='\e[0;33m';
Blu='\e[0;34m';


[ ! -z $1 ] && [ -r $1 ] || { echo "Need a readable install directory to use"; exit 1; }

export INSTALL_DIR="$1"

echo "Running tests, using $1 as install directory"

for d in `find . -type d`; do
    if [ $d = "." ] || [ $d = ".." ] ; then continue; fi
    echo -e ${Yel}"Will run test:${RCol} $d"
    pushd $d
    [ -x run_test.sh ] || { echo "Not run_test.sh files in subdirectory"; exit 1; }
    ./run_test.sh || { echo -e ${Red}"Test failed"${RCol} ; exit 1; }
    echo -e ${Gre}"Test $d succeded"${RCol}
    popd
done
pwd

echo -e ${Gre}"All tests passed !"${RCol}

exit 0
