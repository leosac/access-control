#!/bin/bash

export RPI_ROOTFS="$HOME/Documents/rpi/rootfs2"
export RPI_IP=192.168.0.15
export RPI_TOOLS=$HOME/Documents/rpi/TOOLS
export C_CROSS_COMPILER=$RPI_TOOLS/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc
export CXX_CROSS_COMPILER=$RPI_TOOLS/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++

function get_rpi_tools()
{
    if [ ! -d $RPI_TOOLS ] ; then
	mkdir $RPI_TOOLS || { echo "Failed to create RPI_TOOLS directory: $RPI_TOOLS"; exit -1; }
	git clone git://github.com/raspberrypi/tools.git --depth 1 $RPI_TOOLS
	if [ ! 0 -eq $? ]; then echo "Failed to clone"; exit -1; fi
	return
    else
	echo "RPI_TOOLS already exists... updating"
	pushd $RPI_TOOLS || { echo "Failure"; exit -1; }
	git pull ||  { echo "Failure"; exit -1; }
	popd
	return 
    fi
}

function readme()
{
    echo "Usage: ./build setup | update"
}

# fix some symlinks
function fix_links()
{
    pushd $RPI_ROOTS || { echo "Failure"; exit -1; }

    LDL_LINK=`find usr/ -name libdl.so`
    if [ ! readlink -e $LDL_LINK ] ; then
	"libdl.so link is broken... attempting to fix"
	unlink $LDL_LINK
	ln -s $RPI_ROOTFS/lib/arm-linux-gnueabihf/libdl.so.2 $LDL_LINK
    fi
    popd
}

function update()
{
    echo "Updating RPI libraries and usr directories"
    rsync -rl --delete-after --safe-links --exclude /usr/bin pi@$RPI_IP:/{lib,usr} $RPI_ROOTFS
    if [ $? -ne 0 ] ; then
	echo "Error while syncing..."
	exit 1;
    fi
    fix_links
}

function setup()
{
    { mkdir build && cd build;} || { echo "Failed to create file";  exit -1 ;}

    if [ $1 = "cross" ] ; then
	echo "Setting up project for cross compilation."
	cat > cross.cmake <<EOF
#toolchain for cross compilation                                                                                   c#make_minimum_required(VERSION 2.8.8 FATAL_ERROR)
 
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER \$ENV{C_CROSS_COMPILER})
SET(CMAKE_CXX_COMPILER \$ENV{CXX_CROSS_COMPILER})
SET(CMAKE_SYSROOT \$ENV{RPI_ROOTFS})

INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_SYSROOT}/usr/local/include" "${CMAKE_SYSROOT}/usr/include")
LINK_DIRECTORIES(${CMAKE_SYSROOT}/lib ${CMAKE_SYSROOT}/usr/lib ${CMAKE_SYSROOT}/usr/local/lib)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

EOF
	CROSS_CMAKE=`pwd`/cross.cmake
	if [ ! -e $CROSS_CMAKE ]; then
	    echo "Error"
	    exit 1
	fi
	cmake -DCMAKE_TOOLCHAIN_FILE=$CROSS_CMAKE \
	    -DLEOSAC_BUILD_TESTS=1 \
	    -DZMQPP_BUILD_STATIC=0 \
	    -DZEROMQ_LIB_DIR=$RPI_ROOTFS/usr/local/lib \
	    -DZEROMQ_INCLUDE_DIR=$RPI_ROOTFS/usr/local/include \
	    -DCMAKE_BUILD_TYPE=Debug \
	    ..
    else
	cmake -DLEOSAC_BUILD_TESTS=1 \
	    -DZMQPP_BUILD_STATIC=0 \
	    -DCMAKE_BUILD_TYPE=Debug \
	    ..
    fi
    
    { make VERBOSE=1 -j8  && ctest && echo "Build and test succesfull"; } || { echo "Failure"; exit -1; }
}

if [ $# -lt 1 ]
then
    echo "Not enough arguments"
    readme;
    exit 1;
fi

## clone or pull cross compile tool
get_rpi_tools

if [ $1 = "setup" ] ; then
    if [ -d "build" ]
    then
	echo "Build dir already exists... delete first"
	exit 1
    fi
    echo "Setting up build directory"
    setup $2
    exit 0
fi

if [ $1 = "update" ] ; then
    update
    exit 0
fi

exit 0
