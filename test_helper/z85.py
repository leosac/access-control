#!/usr/bin/env python

import zmq.utils.z85 as z85
import sys

verbose = True

def decode(data):
    if verbose:
        print "Decoding {" + data + "} (total " + str(len(data)) + " bytes)"
        print z85.decode(data)
    else:
        sys.stdout.write(z85.decode(data))

def encode(data):
    data_bin = bytes(data)
    while len(data_bin) % 4:
        data_bin = data_bin + "\0"
    if verbose:
        print "Encoding {" + data_bin + "} (total " + str(len(data_bin)) + " bytes)"
        print z85.encode(data_bin)
    else:
        sys.stdout.write(z85.encode(data_bin))

def main(ac, av):
    count = 1
    global verbose
    if ac > 2 and av[count] == "-s":
        verbose = False
        count = count + 1

    if ac > 1 + count and av[count] == "decode":
        return decode(av[count + 1])
    if ac > 1 + count and av[count] == "encode":
        return encode(av[count + 1])
    print "Usage ./z85.py [-s] encode|decode args"
    return 1

if __name__ == "__main__":
    ret = main(len(sys.argv), sys.argv)
    exit(ret)
