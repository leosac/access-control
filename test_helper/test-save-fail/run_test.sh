#!/bin/bash
#
# Test the SAVE command in failure path
# We toggle the config file Read-Only and make sure we fail to SAVE.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

set -e
set -x

## we include the remote_control.py script
## to make use of some of its code.
cp "$SCRIPT_DIR/../../scripts/remote_control.py" this_test

config_file="$TMP_DIR/this_test/test-save-fail.xml"

chmod 400 $config_file;
chattr +i "$config_file"

(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

sleep 3s;

pushd "$TMP_DIR/this_test"
python test-save.py || { echo "Failure"; ls -l $config_file ; cat $config_file; exit 1; }
popd

kill $(cat pid-file)

while [ ! -r exit-status ] || [ ! -n $(cat exit-status) ];
do
    sleep 1;
done;

[ $(cat exit-status) -eq 0 ] || { exit 1; }

exit 0
