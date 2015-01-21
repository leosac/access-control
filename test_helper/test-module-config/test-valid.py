#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello, querying config for the MONITOR module"

    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "module_config", "MONITOR"]
    ret = remote_control.run(p)

    print p

    if len(ret) >= 3 and ret[0] == "OK" and ret[1] == "MONITOR":
        return 0;
    print "Didn't succeed for valid module"
    return 1

if __name__ == "__main__":
    ret = main()
    exit(ret)
