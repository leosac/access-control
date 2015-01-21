#!/bin/bash
#
# Test the MODULE_CONFIG command.
# One running Leosac. We use the remote_control script to query the results.
#

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

set -e
set -x

## we include the remote_control.py script
## to make use of some of its code.
cp "$SCRIPT_DIR/../../scripts/remote_control.py" .
ls

(valgrind --error-exitcode=42 ./install/bin/leosac -k $config_file > leosac-log &
    echo $! > pid-file;  wait $! && echo $? > exit-status) &

python "$SCRIPT_DIR/test-valid.py"
python "$SCRIPT_DIR/test-invalid.py"

kill $(cat pid-file)

while [ ! -r exit-status ] || [ ! -n $(cat exit-status) ];
do
    sleep 1;
done;

[ $(cat exit-status) -eq 0 ] || { exit 1; }

exit 0
