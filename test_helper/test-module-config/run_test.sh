#!/bin/bash
#
# Test the MODULE_CONFIG command.
# One running Leosac. We use the remote_control script to query the results.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

## we include the remote_control.py script
## to make use of some of its code.

cp "$SCRIPT_DIR/../../scripts/remote_control.py" .
python "$SCRIPT_DIR/test.py"
