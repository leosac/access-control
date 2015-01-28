#!/bin/bash
#
# Test the GENERAL_CONFIG command.
# One running Leosac. We use the remote_control script to query the results.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

set -e
set -x

## we include the remote_control.py script
## to make use of some of its code.
cp "$SCRIPT_DIR/../../scripts/remote_control.py" this_test

config_file="$TMP_DIR/this_test/test-general-config.xml"

(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

sleep 5s
pushd "$TMP_DIR/this_test"
python test-general-config.py
popd

sleep 5s
kill $(cat pid-file)

while [ ! -r exit-status ] || [ ! -n $(cat exit-status) ];
do
    sleep 1;
done;

[ $(cat exit-status) -eq 0 ] || { exit 1; }

exit 0
