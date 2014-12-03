#!/bin/bash
#
# Test that unkown card generate deny.
# Test that not-allowed card generate deny.
# Test that valid card trigger success. 
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

config_file="$TMP_DIR/this_test/test-auth-file1.xml"
cp $TMP_DIR/this_test/auth-file.xml $TMP_DIR

## Send 3 unkown cards, 1 unauthorized and 2 that have access
($INSTR send_card 11:22:33:44
    sleep 3s
    $INSTR send_card 80:83:a0:40
    sleep 3s
    $INSTR send_card 15:22:33:44
    sleep 3s
    $INSTR send_card 0a:22:33:44
    sleep 3s
    $INSTR send_card 80:81:61:40
    sleep 3s
    $INSTR send_card 80:83:a0:40
    sleep 3s
    kill $(cat pid-file)
)&

## start leosac and wait for return value
## Use custom work directory to access the factory config file
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status)

log_file="leosac-log"

## This is how the monitor print a frame from auth backend.
## 0SSi2 is the z85 encoding of the failure (0x02)
fail_auth_string="F1: {0SSi2} ;"
success_auth_string="F1: {0rr91} ;"

grepped_fail=`grep "{S_AUTH_CONTEXT_1}" $log_file | grep "$fail_auth_string" | wc -l`
grepped_success=`grep "{S_AUTH_CONTEXT_1}" $log_file | grep "$success_auth_string" | wc -l`

[ $grepped_fail -eq 4 ] || fail "Didnt fail 4 times"
[ $grepped_success -eq 2 ] || fail "Didnt succeed 2 times"

die 0
