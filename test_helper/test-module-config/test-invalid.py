#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello, I will query the config of an invalid module."
    
    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "module_config", "INVALID_MODULE"]
    ret = remote_control.run(p)

    print ret
    if len(ret) != 2 or ret[0] != "KO":
        print "Didn't fail for invalid module..."
        return 1
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
