#!/bin/bash

[ ! -z $1 ] || exit 1

ping -c 1 -W 3 $1
exit $?
