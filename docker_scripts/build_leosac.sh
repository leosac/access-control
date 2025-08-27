#!/bin/bash
#
# This script will build leosac and copy resulting deb file

set -x
set -e

APP_PATH=/tmp/leosac
DEB_PATH=${APP_PATH}/build/packages/${DISTRIB}/${TARGETPLATFORM}
mkdir -p $DEB_PATH

pushd $APP_PATH/python
if [ $DISTRIB = "debian-bullseye" ]; then
   pip3 install -e .
else
   pip3 install -e . --break-system-packages
fi
popd;

pushd $APP_PATH
./docker_scripts/makepkg.sh
LEOSAC_DEB=`find ../* -maxdepth 0 -name leosac_*.deb`
mv $LEOSAC_DEB $DEB_PATH

popd;
