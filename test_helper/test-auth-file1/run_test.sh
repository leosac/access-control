#!/bin/bash
#
# Test that unkown card generate deny.
# Test that not-allowed card generate deny.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

config_file="$TMP_DIR/this_test/test-auth-file1.xml"
cp $TMP_DIR/this_test/auth-file.xml $TMP_DIR

## Send 3 unkown cards and 1 unauthorized
($INSTR send_card 11:22:33:44
    sleep 0.2s
    $INSTR send_card 15:22:33:44
    sleep 0.2s
    $INSTR send_card 0a:22:33:44
    sleep 0.2s
    $INSTR send_card 80:81:61:40
    sleep 3
    kill `pidof leosac`
)&

## start leosac and wait for return value
## Use custom work directory to access the factory config file
(./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status)

log_file="leosac-log"

## This is how the monitor print a frame from auth backend.
## 0SSi2 is the z85 encoding of the failure (0x02)
fail_auth_string="F1: {0SSi2} ;"
grepped_fail=`grep "{S_AUTH_CONTEXT_1}" $log_file | grep "$fail_auth_string" | wc -l`

[ $grepped_fail -eq 4 ] || fail "Didnt fail 4 times"

die 0
