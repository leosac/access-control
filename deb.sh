#!/bin/bash
#
# This script generate debian package for Leosac.
#

#
# SUBROUTINES
#

# Unused function
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

function clone()
{
    git clone $1
    pushd leosac
    git checkout $2;
    git submodule init;
    git submodule update;
    popd
}

usage()
{
cat <<EOF

Usage: $0 [-h] [-u GIT REPO ] [-b GIT BRANCH ]

OPTIONS:
   -h      Show this message and quit
   -u      Full url to a Leosac git repo
   -b      Git branch name to checkout

EOF
}

#
# BEGIN MAIN PROGRAM
#

# Set default values
url="https://github.com/leosac/leosac.git"
branch="develop"

while getopts "hu:b:" OPTION
do
     case $OPTION in
         h)
             usage
             exit 0
             ;;
         u)
             url="$OPTARG"
             ;;
         b)
             branch="$OPTARG"
             ;;
         *)
             usage
             exit 50
             ;;
     esac
done

# Check to see if this script has access to all the commands it needs
for CMD in cp debuild egrep git grep mk-build-deps mv sudo tar; do
  type $CMD 2>&1 > /dev/null

  if [ $? -ne 0 ]; then
    echo
    echo "ERROR: The script cannot find the required command \"${CMD}\"."
    echo
    exit 99
  fi
done

TMP_DIR=$(mktemp -d)
cd $TMP_DIR
echo $TMP_DIR

clone $url $branch

MAJOR=`grep "DLEOSAC_VERSION_MAJOR=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`
MINOR=`grep "DLEOSAC_VERSION_MINOR=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`
PATCH=`grep "DLEOSAC_VERSION_PATCH=" leosac/CMakeLists.txt | egrep -o '([0-9]+)'`

name="leosac_${MAJOR}.${MINOR}.${PATCH}"
mv leosac $name

tar czf leosac_${MAJOR}.${MINOR}.${PATCH}.orig.tar.gz $name

cd $name

#copy existing pkg/debian directory (maintained in repos, with patches)
cp -R pkg/debian .

# Auto-install build dependencies using the debian control file
sudo mk-build-deps -ir -t "apt-get -o Debug::pkgProblemResolver=yes \
    --no-install-recommends --no-upgrade -y" \
    ./debian/control

if [ -z ${DEB_BUILD_OPTIONS} ] ; then
    export DEB_BUILD_OPTIONS="parallel=3"
fi
debuild -b -us -uc

RESULT="$?"

if [ "$RESULT" == "0" ]; then
    echo
    echo "The build succeeded."
    echo "Debian package files can be found here: ${TMP_DIR}"
    echo
fi
