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

total_test=`find . -maxdepth 1 -mindepth 1 -type d | wc -l`
count=0
for d in `find . -maxdepth 1 -mindepth 1 -type d`; do
    count=$(($count+1))
    echo -e ${Yel}"Will run test:${RCol} $d"
    pushd $d
    [ -x run_test.sh ] || { echo "Not run_test.sh files in subdirectory"; exit 1; }
    ./run_test.sh || { echo -e ${Red}"Test failed"${RCol} ; exit 1; }
    
    if [ ! -z "`pidof leosac`" ]; then
	echo ${Red}"For some reason a leosac is still active...${RCol}"
	for p in `pidof leosac`; do
	    echo ${Red}"Killing $p"${RCol}
	    kill -9 $p
	done
    fi

    echo -e ${Gre}"Test $d succeeded ($count / $total_test)"${RCol}
    popd
done
pwd

echo -e ${Gre}"All $count tests passed !"${RCol}

exit 0
