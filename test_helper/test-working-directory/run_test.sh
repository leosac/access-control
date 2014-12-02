#!/bin/bash

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

## should fail, no valid path to plugin
if ./install/bin/leosac/ -k $TMP_DIR/this_test/amd64.xml > leosac-log; then
    fail "Leosac exited with status 0 but should have failed"
fi

## should work because we use the plugin dir as workding dir
(sleep 3s; kill `pidof leosac`) &
if ! ./install/bin/leosac -k $TMP_DIR/this_test/amd64.xml \
    -d $TMP_DIR/install/lib/leosac > leosac-log; then
    fail "Leosac failed, but should have succeded"
fi

die 0
