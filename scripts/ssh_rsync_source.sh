#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<remote>"
    echo ""
    echo "Sync remote leosac source folder to local machine via ssh (useful when developing on an x86 platform and building on ARM)"
    echo ""
    echo "Example:" $0 "user@192.168.0.42:/home/user/leosac/"
    exit 1
fi

PATHTOTARGET='leosac/'

rsync -rav --delete --exclude '.git' --exclude 'build' --exclude 'doc' -e ssh $1 ${PATHTOTARGET}
