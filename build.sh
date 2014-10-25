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
    echo "Usage: ./build f|s setup | update"
    echo "f -> force: will delete build directory. Will not pause."
    echo "s -> safe:"
}

# fix some symlinks
function fix_links()
{
    pushd $RPI_ROOT || { echo "Failure"; exit -1; }

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
    rsync -vrl --delete-after --safe-links --exclude /usr/bin pi@$RPI_IP:/{lib,usr} $RPI_ROOTFS
    if [ $? -ne 0 ] ; then
	echo "Error while syncing..."
	exit 1;
    fi
    fix_links
}

## Build libzmq and install to RPI_ROOT
function build_libzmq()
{
    git clone git://github.com/zeromq/libzmq.git
    pushd libzmq;
    git checkout e9b9860752ffac1a561fdb64f5f72bbfc5515b34
    ./autogen.sh || { echo "libzmq autogen failed"; exit -1 ;}
    ./configure --with-sysroot=$RPI_ROOTFS \
	--host=arm-linux-gnueabihf \
	--prefix=$RPI_ROOTFS/usr \
	CXX=$CXX_CROSS_COMPILER CC=$C_CROSS_COMPILER \
	|| { echo "libzmq configure failed"; exit -1 ;}
    make -j10 	|| { echo "libzmq build failed"; exit -1 ;}
    make install || { echo "libzmq install failed"; exit -1 ;}
    popd
}


## Build libgest from RPIROOT gtest source and install
## binary in rpi root.
## $1 = f | s
function build_gtest()
{
    GTEST_SRC=$RPI_ROOTFS/usr/src/gtest
    if [ ! -d $GTEST_SRC ]; then
	echo "Cannot find gtest sources"
    fi
    
    pushd $GTEST_SRC || { echo "$GTEST_SRC doesn't seem to exist"; exit -1 ; }
    if [ -d $GTEST_SRC/build ] ; then
	echo "A google test build directory exist already."
	if [ $1 = "f" ] ; then
	    echo "Force mode... removing it";
	    rm -rf $GTEST_SRC/build;
	else
	    exit -1;
	fi
    fi

    { mkdir -p build && pushd build ; } || { echo "gtest error"; exit -1 ; }
    { cmake ../ -DCMAKE_TOOLCHAIN_FILE=$CROSS_CMAKE && make ;} || { echo "gtest build error"; exit -1 ; }
    cp libgtest_main.a libgtest.a $RPI_ROOTFS/usr/lib || { echo "failed to install gtest to rpi rootfs"; exit -1 ;}
    popd; popd;
}

function toolchain_file_create()
{
    cat > cross.cmake <<EOF
#toolchain for cross compilation                                                                                   c#make_minimum_required(VERSION 2.8.8 FATAL_ERROR)
 
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER \$ENV{C_CROSS_COMPILER})
SET(CMAKE_CXX_COMPILER \$ENV{CXX_CROSS_COMPILER})
SET(CMAKE_SYSROOT \$ENV{RPI_ROOTFS})

INCLUDE_DIRECTORIES(SYSTEM ${CMAKE_BINARY_DIR}/libzmq/include "${CMAKE_SYSROOT}/usr/local/include" "${CMAKE_SYSROOT}/usr/include")
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/libzmq/.libs ${CMAKE_SYSROOT}/lib ${CMAKE_SYSROOT}/usr/lib ${CMAKE_SYSROOT}/usr/local/lib)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

EOF
    export CROSS_CMAKE=`pwd`/cross.cmake
    if [ ! -e $CROSS_CMAKE ]; then
	echo "Error creating CMake toolchain file"
	exit 1
    fi
}

## $1 -> f | s (force or safe)
## $2 -> compile mode (normal | cross)
function setup()
{
    { mkdir build && cd build;} || { echo "Failed to create file";  exit -1 ;}

    toolchain_file_create    
    build_libzmq $1
    build_gtest $1

    if [ $2 = "cross" ] ; then
	echo "Setting up project for cross compilation."
	# wait for user input
	if [ $1 = "s" ] ; then read; fi
	    
	cmake -DCMAKE_TOOLCHAIN_FILE=$CROSS_CMAKE \
	    -DLEOSAC_BUILD_TESTS=1 \
	    -DZMQPP_BUILD_STATIC=0 \
	    -DZEROMQ_LIB_DIR=`pwd`/libzmq/.libs/ \
	    -DZEROMQ_INCLUDE_DIR=`pwd`/libzmq/include \
	    -DCMAKE_BUILD_TYPE=Debug \
	    ..
    else
	cmake -DLEOSAC_BUILD_TESTS=1 \
	    -DZMQPP_BUILD_STATIC=0 \
	    -DCMAKE_BUILD_TYPE=Debug \
	    ..
    fi
    if [ $1 = "s" ]; then read; fi
    
    { make -j8  && ctest && echo "Build and test succesfull"; } || { echo "Failure"; exit -1; }
}

if [ $# -lt 2 ]
then
    echo "Not enough arguments"
    readme;
    exit 1;
fi

## clone or pull cross compile tool
get_rpi_tools

if [ $2 = "setup" ] ; then
    if [ -d "build" ]
    then
	if [ $1 = "f" ]; then
	    rm -rf build;
	else
	    echo "Build dir already exists... delete first"
	    exit 1
	fi
    fi
    echo "Setting up build directory"
    COMPILE_MODE="normal"
    if [ $# -eq 3 ]; then COMPILE_MODE=$3; fi;
    setup $1 $COMPILE_MODE
    exit 0
fi

if [ $2 = "update" ] ; then
    update
    exit 0
fi

exit 0
