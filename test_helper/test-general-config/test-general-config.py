#!/usr/bin/env python

import remote_control
import sys

def main():
    print "Hello, I will query the general config information"
    
    p = ["", "127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT", "general_config"]
    ret = remote_control.run(p)

    print ret
    if len(ret) != 2 or ret[0] != "OK":
        print "Failed to retrieve general config."
        return 1

    cfg = ret[1];
    expected = """<?xml version="1.0" encoding="utf-8"?>
<plugin_directories>
	<plugindir>./install/lib/leosac</plugindir>
	<plugindir>./install/lib/leosac/auth</plugindir>
</plugin_directories>
"""
    ## test the contain of the response.
    if ret[1] != expected:
        print "Invalid content in response"
        return 1
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
