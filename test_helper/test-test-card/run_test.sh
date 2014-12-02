#!/bin/bash

TMP_DIR=$(mktemp -d)
set -x
set -e

echo "Working in $TMP_DIR"
function die()
{
    rm -rf $TMP_DIR
    exit $1
}

## check that we are in leosac/test_helper and that we run the script from here
## ./test_working_directory.
[ `dirname $0` == "." ] || die -1
ROOT_TEST_DIR=`pwd`

cp -R ../../build/. $TMP_DIR
cp ./test-test-card.xml $TMP_DIR

cd $TMP_DIR;


## send test-card id
INSTR="$ROOT_TEST_DIR/../instrumentation_client.py /tmp/leosac-ipc"

($INSTR send_card 40:a0:83:80
sleep 3
kill `pidof leosac`
)&

#start leosac and wait for return value
(./leosac -k $TMP_DIR/test-test-card.xml > leosac-log &
echo $! > pid-file;  wait $! && echo $? > exit-status)

## check test sequence was run.
if ! grep "\[debug\]" $TMP_DIR/leosac-log | grep "Running test sequence..."; then
    echo "Test sequence wasn't run";
    die 1
fi

die 0
