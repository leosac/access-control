#!/bin/bash

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

## should fail, no valid path to plugin
if ./install/bin/leosac/ -k $TMP_DIR/this_test/amd64.xml; then
    echo "Leosac exited with status 0 but should have failed"
    die 1
fi

## should work because we use the plugin dir as workding dir
(sleep 3s; kill `pidof leosac`) &
if ! ./install/bin/leosac -k $TMP_DIR/this_test/amd64.xml \
    -d $TMP_DIR/install/lib/leosac ; then
    echo "Leosac failed, but should have succeded"
    die 1
fi

die 0
