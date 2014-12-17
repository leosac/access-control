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
        print "Running 'module_list;"
        self.socket_.send("MODULE_LIST")
        ret = self.socket_.recv_multipart()
        print ret

def print_usage():
    print "Usage: ./remote_control tcp_endpoint server_key command [params]"

def main():
    if len(sys.argv) < 4:
        print_usage()
        return -1
    tcp_endpoint = sys.argv[1]
    server_key = sys.argv[2]

    context = zmq.Context.instance()
    dealer = context.socket(zmq.DEALER)

    #random key as the server doesn't check those
    client_public, client_secret = zmq.curve_keypair()
    dealer.curve_serverkey = server_key
    dealer.curve_publickey = client_public
    dealer.curve_secretkey = client_secret

    connect_str = "tcp://" + str(tcp_endpoint)
    dealer.connect(connect_str)
    print "Connected to " + connect_str

    ch = CommandHandler(dealer, sys.argv);
    ch.handle_command(sys.argv[3])

    
    time.sleep(5)
    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
