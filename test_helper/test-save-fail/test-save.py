#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello, will attempt to SAVE"

    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "save"]
    ret = remote_control.run(p)

    print ret

    if len(ret) == 2 and ret[0] == "KO":
        return 0;
    print "Looks like SAVE succeeded but should have failed"
    return 1

if __name__ == "__main__":
    ret = main()
    exit(ret)
