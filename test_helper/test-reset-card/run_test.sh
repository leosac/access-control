#!/bin/bash

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

config_file="$TMP_DIR/this_test/test-reset-card.xml"

## Send reset card
($INSTR send_card 40:a0:83:80
    sleep 5s
    kill $(cat pid-file)
)&

## Copy our own factory dummy config on top of real one (because we cant
## control network if we are not root.
cp ./this_test/factory-kernel.xml ./install/share/leosac/cfg/factory/kernel.xml

## Make sure factory conf and current are different, otherwise
## Test will be useless
if diff $config_file ./install/share/leosac/cfg/factory/kernel.xml > /dev/null; then
    fail "Factory and current config file are not different."
fi

## start leosac and wait for return value
## Use custom work directory to access the factory config file
(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file -d ./install > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status)

if ! diff $config_file ./install/share/leosac/cfg/factory/kernel.xml > /dev/null; then
    fail "Factory and current config should be identical, but they are not".
fi

die 0
