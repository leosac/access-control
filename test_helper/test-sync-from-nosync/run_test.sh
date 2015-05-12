#!/bin/bash
#
# Test the SYNC_FROM command with the <no_import> tag for
# exluding some module from the synchronisation
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

SUM=$(md5sum $config_file)

(${REMOTE_CONTROL} "127.0.0.1:12345" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' "sync_from" "tcp://127.0.0.1:12346" 'TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT' 1 1;
    sleep $SLEEP_TIME
    kill $(cat pid-file)
)&

## start leosac and wait for return value
## Use custom work directory to access the factory config file
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

(valgrind --error-exitcode=42 ./install/bin/leosac -k $source_cfg_file > leosac-log-2 &
    echo $! > pid-file-2;  wait $! && echo $? > exit-status-2) &

sleep 15s
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


### We use autosave=true to overwrite config file.
### Checksum should then change.
SUM_AFTER_UPDATE=$(md5sum $config_file)

if [ "${SUM_AFTER_UPDATE}" = "${SUM}" ] ; then
    echo "Config file checksum did not change. Something is wrong."
    exit 1
fi

## Also make sure we cannot grep "MY_SUPER_WIEGAND_1" as this would mean
## that this module was synchronized while it shouldn't.
if grep 'MY_SUPER_WIEGAND_1' $config_file ; then
echo "Looks like module wiegand synchronize while it should'nt have"
echo "Dumping cfg file"
cat $config_file;
exit 1;
fi

die 0
