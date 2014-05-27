#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<remote>"
    echo ""
    echo "Sync remote osac source folder to local machine via ssh (useful when developing on an x86 platform and building on ARM)"
    echo ""
    echo "Example:" $0 "user@192.168.0.42:/home/user/osac/"
    exit 1
fi

PATHTOTARGET='osac/'

rsync -rav --delete --exclude 'build' --exclude '.git' -e ssh $1 ${PATHTOTARGET}
