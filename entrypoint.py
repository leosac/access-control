#!/usr/bin/env python3

import os
import sys

def build_fresh():
    os.system("cd /leosac_arm_build  && \
    cmake -DCMAKE_SYSROOT=/opt/rpi_fakeroot -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=/leosac/cmake/rpi-cross.cmake /leosac && \
    make -j6")

def build():
    os.system("cd /leosac_arm_build && make -j6")

def deploy(target_ip):
    os.system("scp -r /leosac_arm_build root@{}:/leosac")
    os.system("scp -r /opt/rpi_fakeroot root@{}:/leosac_fakeroot")
    
def main():
    if len(sys.argv) >= 2:
        if sys.argv[1] == 'deploy':
            if len(sys.argv) >= 3:
                deploy(sys.argv[2])
            else:
                print("Usage ./entrypoint deploy IP")
        elif sys.argv[1] == 'build_fresh':
            build_fresh()
        elif sys.argv[1] == 'build':
            build()
    else:
         print("Usage ./entrypoint.py deploy|build_fresh|build")   

if __name__ == "__main__":
    main()
