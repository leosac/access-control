#!/usr/bin/env python3

import os
import sys

def throw_on_error(st):
    if os.WIFEXITED(st) and os.WEXITSTATUS(st) == 0:
        return
    elif os.WIFEXITED(st):
        raise RuntimeError("Error when executing script. Return value: {}".format(
            os.WEXITSTATUS(st)))
    else:
        raise RuntimeError("Error when executing script. Killed by signal {}".format(
            os.WSTOPSIG(st)))

def copy_libstdcxx(target_ip):
    """
    We copy libstdc++ to the fakeroot on the raspberry
    """
    opt = '-o StrictHostKeyChecking=no -i /cross-compile-resources/deploykey'
    st = os.system("scp {} /usr/arm-linux-gnueabihf/lib/libstdc++.so.6.0.22 root@{}:/opt/leosac_fakeroot/lib".format(opt, target_ip))
    throw_on_error(st)
    st = os.system("ssh {} root@{} 'ln -f -s /opt/leosac_fakeroot/lib/libstdc++.so.6.0.22 /opt/leosac_fakeroot/lib/libstdc++.so.6'".format(opt, target_ip))
    throw_on_error(st)

def build_fresh():
    st = os.system("cd /leosac_arm_build  && \
    cmake -DCMAKE_SYSROOT=/opt/rpi_fakeroot -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=/leosac/cmake/rpi-cross.cmake /leosac && \
    make -j6")
    throw_on_error(st)

def build():
    st = os.system("cd /leosac_arm_build && make -j6")
    throw_on_error(st)
    
def deploy_all(target_ip):
    """
    Deploy both the complete leosac build output along with the fakeroot
    containing cross compiled dependencies.
    """
    opt = '-a --delete -r -v -e "ssh -o StrictHostKeyChecking=no -i /cross-compile-resources/deploykey" '
    st = os.system("rsync {} /leosac_arm_build/ root@{}:/opt/leosac".format(opt, target_ip))
    throw_on_error(st)
    os.system("rsync {} /opt/rpi_fakeroot/ root@{}:/opt/leosac_fakeroot".format(opt, target_ip))
    throw_on_error(st)
    copy_libstdcxx(target_ip)

def build_and_deploy(target_ip):
    """
    Build (incremental compilation) and deploy a lightweight version
    of the Leosac build output. Leosac's libraries and binary are
    deployed but not other artefacts such as object files.
    
    The 'environement' (fakeroot folder) is NOT deployed
    """
    build()
    opt= '-a --delete -r -v -e "ssh -o StrictHostKeyChecking=no -i /cross-compile-resources/deploykey" '
    opt += "--exclude '*.o'"
    st = os.system("rsync {} /leosac_arm_build/ root@{}:/opt/leosac".format(opt, target_ip))
    throw_on_error(st)
    copy_libstdcxx(target_ip)

def main():
    if len(sys.argv) >= 2:
        if sys.argv[1] == 'build_and_deploy':
            if len(sys.argv) >= 3:
                build_and_deploy(sys.argv[2])
            else:
                print("Usage ./entrypoint build_and_deploy IP")
        elif sys.argv[1] == 'deploy_all':
            if len(sys.argv) >= 3:
                deploy_all(sys.argv[2])
            else:
                print("Usage ./entrypoint deploy_all IP")
        elif sys.argv[1] == 'build_fresh':
            build_fresh()
        elif sys.argv[1] == 'build':
            build()
        else:
            print("Usage ./entrypoint.py deploy_all|build_fresh|build")
    else:
         print("Usage ./entrypoint.py deploy_all|build_fresh|build")   

if __name__ == "__main__":
    main()
