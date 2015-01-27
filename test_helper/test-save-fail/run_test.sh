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


## Use a RO filesystem to store the config file.
## This way, we are sure that SAVE will fail.
dd if=/dev/zero of=${TMP_DIR}/block bs=4096 count=1024
mkfs.ext4 ${TMP_DIR}/block
mkdir ${TMP_DIR}/mnt
mount -o loop,rw ${TMP_DIR}/block ${TMP_DIR}/mnt
cp $config_file $TMP_DIR/mnt
mount -o remount,ro ${TMP_DIR}/block ${TMP_DIR}/mnt
config_file="${TMP_DIR}/mnt/test-save-fail.xml"

(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

sleep 8s;

pushd "$TMP_DIR/this_test"
python test-save.py || { echo "Failure"; ls -l $config_file ; cat $config_file; exit 1; }
popd

sleep 4s

kill $(cat pid-file)

while [ ! -r exit-status ] || [ ! -n $(cat exit-status) ];
do
    sleep 1;
done;

[ $(cat exit-status) -eq 0 ] || { exit 1; }

exit 0
