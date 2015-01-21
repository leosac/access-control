#!/bin/bash
#
# Test the MODULE_LIST command.
# One running Leosac. We use the remote_control script to query the results.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

set -e
set -x

config_file="$TMP_DIR/this_test/test-module-list.xml"

(${REMOTE_CONTROL} "127.0.0.1:12345" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' "module_list" > remote_result
    sleep $SLEEP_TIME
    kill $(cat pid-file)
)&


## start leosac and wait for return value
## Use custom work directory to access the factory config file
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status)

if ! grep "MONITOR" remote_result ; then
   fail "MONITOR module should be reported present."
fi

if ! grep "WIEGAND_READER" remote_result ; then
   fail "WIEGAND_READER module should be reported present."
fi

[ $(cat exit-status) -eq 0 ] || fail "Non zero return code"

die 0
