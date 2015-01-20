#!/bin/bash

set -x
set -e 

echo "Running user command..."
bash -c "$@"

exit $?

