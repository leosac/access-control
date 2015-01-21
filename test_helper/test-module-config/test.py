#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello"
    
    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "module_config", "INVALID_MODULE"]
    remote_control.run(p)

    if p[0] == "KO":
        return 0;
    print "Didnt fail for invalid module..."
    return 1

if __name__ == "__main__":
    ret = main()
    exit(ret)
