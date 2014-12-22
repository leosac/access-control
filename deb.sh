#!/bin/bash
#
# This script generate debian package for Leosac.
#

TMP_DIR=$(mktemp -d)

function die()
{
    if [ -z $1 ]; then
	echo "No return value..."
	exit 42;
    fi

    echo $2
    rm -rf $TMP_DIR
    exit $1
}
cd $TMP_DIR

echo $TMP_DIR

function clone()
{
    git clone https://github.com/islog/leosac.git
    pushd leosac
    git submodule init;
    git submodule update;
    popd
}

clone

MAJOR=`grep "DLEOSAC_VERSION_MAJOR=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`
MINOR=`grep "DLEOSAC_VERSION_MINOR=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`
PATCH=`grep "DLEOSAC_VERSION_PATCH=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`

name="leosac_${MAJOR}.${MINOR}.${PATCH}"
mv leosac $name

tar czf leosac_${MAJOR}.${MINOR}.${PATCH}.orig.tar.gz $name

cd $name

#copy existing pkg/debian directory (maintained in repos, with patches)
cp -R pkg/debian .

export DEB_BUILD_OPTIONS="parallel=3"
debuild -us -uc

