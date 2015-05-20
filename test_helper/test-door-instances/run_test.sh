#!/bin/bash

[ -r ../shell_helper.sh ] || { echo "Cannot source shell_helper.sh"; exit -1; }
source ../shell_helper.sh

python this_test/run_test.py
