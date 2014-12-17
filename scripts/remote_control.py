#!/usr/bin/env python

import time
import sys
import zmq

class CommandHandler(object):
    def __init__(self, sock, argv):
        self.socket_ = sock
        self.argv_ = argv

    def handle_command(self, cmd):
        if (cmd == "module_list"):
            self.handle_module_list();

    def handle_module_list(self):
        self.socket_.send("MODULE_LIST")
        test = self.socket_.recv()
        print test

def print_usage():
    print "Usage: ./remote_control tcp_endpoint command [params]"

def main():
    if len(sys.argv) < 3:
        print_usage()
        return -1
    tcp_endpoint = sys.argv[1]

    context = zmq.Context.instance()
    dealer = context.socket(zmq.DEALER)

    connect_str = "tcp://" + str(tcp_endpoint)
    dealer.identity = "bla"
    dealer.connect(connect_str)
    print "Connected to " + connect_str

    ch = CommandHandler(dealer, sys.argv);
    ch.handle_command(sys.argv[2])

    
    time.sleep(5)
    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
