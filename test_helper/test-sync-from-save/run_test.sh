#!/bin/bash
#
# Test the SYNC_FROM command.
# Two configuration file. One source, and one for the raspi that
# will issue the SYNC_FROM command.
#
# We test the SAVE command too.
#
# This test runs two Leosac.

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

set -e
set -x

config_file="$TMP_DIR/this_test/test-sync-from.xml"

## config file for the source unit
source_cfg_file="$TMP_DIR/this_test/source_cfg.xml"

## We need a dummy run to let autosave_ reformat the config file.
## Otherwise checksum will differ event if the content is semantically the same
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

sleep 10s
kill $(cat pid-file)
sleep 5s


(${REMOTE_CONTROL} "127.0.0.1:12345" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' "sync_from" "tcp://127.0.0.1:12346" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' 0 0;
 ${REMOTE_CONTROL} "127.0.0.1:12345" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' "save" ;
    sleep $SLEEP_TIME ;
    kill $(cat pid-file) ;
)&

SUM=$(md5sum $config_file)

## start leosac and wait for return value
## Use custom work directory to access the factory config file
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

(valgrind --error-exitcode=42 ./install/bin/leosac -k $source_cfg_file > leosac-log-2 &
    echo $! > pid-file-2;  wait $! && echo $? > exit-status-2) &

sleep 15s
## Save should have been processed by now.
SUM_AFTER_SAVE=$(md5sum $config_file)

kill $(cat pid-file)
kill $(cat pid-file-2)

while [ ! -r exit-status ] || [ ! -n $(cat exit-status) ];
do
    sleep 1;
done;

echo "Process finished with status" $(cat exit-status);

while [ ! -r exit-status-2 ] || [ ! -n $(cat exit-status-2) ];
do
    sleep 1;
done;

echo "Process 2 finished with status" $(cat exit-status-2);

if [ "${SUM_AFTER_SAVE}" = "${SUM}" ] ; then
    echo "Config file checksum did not change. Something is wrong."
    exit 1
fi

die 0
