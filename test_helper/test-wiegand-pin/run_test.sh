#!/bin/bash

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

## send 4bits pin-code
(
sleep 10s
$INSTR send_pin_4bits 1234
    sleep $SLEEP_TIME
    kill $(cat pid-file)
)&

#start leosac and wait for return value
(valgrind --error-exitcode=42 ./install/bin/leosac -k $TMP_DIR/this_test/config.xml > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status)


## check test sequence was NOT run.
if ! ( grep "\[debug\]" leosac-log | grep -e ".* 1234$" ) ; then
    fail "Cannot find PIN code.";
fi

die 0
