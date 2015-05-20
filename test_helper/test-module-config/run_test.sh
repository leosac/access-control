#!/bin/bash
#
# Test the MODULE_CONFIG command.
# One running Leosac. We use the remote_control script to query the results.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

python this_test/run_test.py
