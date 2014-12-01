#!/usr/bin/env python

import sys
import zmq

def print_usage():
    print "Usage: ./intrumentation_client leosac-pid"

def main():
    if len(sys.argv) < 2:
        print_usage()
        return -1
    pid = int(sys.argv[1])

    context = zmq.Context.instance()
    dealer = context.socket(zmq.DEALER)

    connect_str = "ipc:///tmp/leosac-ipc/" + str(pid)
    dealer.connect(connect_str)
    print "Connected too " + connect_str
    for i in range(1, 100000):
        dealer.send_string("hello");

    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
