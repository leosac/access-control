#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello"

    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "module_config", "MONITOR"]
    remote_control.run(p)

    if size(p) > 3 and p[0] == "OK" and p[1] == "MONITOR":
        return 0;
    print "Didn't succeed for valid module"
    return 1

if __name__ == "__main__":
    ret = main()
    exit(ret)
