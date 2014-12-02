#!/bin/bash
#
# Source this file in each test runner, as it provides
# re-usable shell functions.
#
# It also set useful variables and sanitize test environment by
# copying binary and config files and making sure the script is
# launch from an appropriate directory.

RCol='\e[0m' # Text Reset
Bla='\e[0;30m';
Red='\e[0;31m';
Gre='\e[0;32m';
Yel='\e[0;33m';
Blu='\e[0;34m';

## Write $@ to stderr
function errcho()
{
    >&2 echo "$@"
}

## Perform cleanup and exit.
## $1 is status code. If not set returns 1
function die()
{
    rm -rf ${TMP_DIR}
    [ ! -z $1 ] && exit $1
    exit 1
}

## Print an error message and die
## $1 is the error message
function fail()
{
    errcho -e ${Red}"Exiting due to fatal error:"${RCol} $1
    die 1
}

## This test working directory
TMP_DIR=$(mktemp -d)

## This test source directory
SCRIPT_DIR=`pwd`

## Instrumentation Client (python script)
INSTR="$SCRIPT_DIR/../instrumentation_client.py /tmp/leosac-ipc"

set -x
set -e

[ `dirname $0` == "." ] || fail "Not started from correct directory"
[ ! -z "$INSTALL_DIR" ] || fail "No install directory"

## setting up working dir
cp -R $INSTALL_DIR $TMP_DIR/install
cp -R $SCRIPT_DIR $TMP_DIR/this_test

## Go to working directory
cd $TMP_DIR
