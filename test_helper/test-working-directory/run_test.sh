#!/bin/bash

TMP_DIR=$(mktemp -d)
set -x
set -e

echo "Working in $TMP_DIR"
function die()
{
    rm -rf $TMP_DIR
    exit $1
}

## check that we are in leosac/test_helper and that we run the script from here
## ./test_working_directory.
[ `dirname $0` == "." ] || die -1

cp -R ../../build $TMP_DIR
cp ../../cfg/dev-test/amd64.xml $TMP_DIR

cd $TMP_DIR;

mkdir -p path/to/wd

## should fail, no valid path to plugin
if ./build/leosac -k $TMP_DIR/amd64.xml; then
    echo "Leosac exited with status 0 but should have failed"
    die 1
fi

## should work because we use the plugin dir as workding dir
(sleep 3s; kill `pidof leosac`) &
if ! ./build/leosac -k $TMP_DIR/amd64.xml -d $TMP_DIR/build ; then
    echo "Leosac failed, but should have succeded"
    die 1
fi

die 0
