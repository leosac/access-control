#!/usr/bin/env python

import time
import sys
import zmq

class CommandHandler(object):
    def __init__(self, sock, argv):
        self.socket_ = sock
        self.argv_ = argv
        self.argv_offset = 4

    def handle_command(self, cmd):
        if (cmd == "module_list"):
            self.handle_module_list();
        if (cmd == "module_config"):
            self.handle_module_config(self.argv_[self.argv_offset])
        if (cmd == "sync_from"):
            self.sync_from(self.argv_[self.argv_offset])

    def handle_module_list(self):
        print "Running 'module_list;"
        self.socket_.send("MODULE_LIST")
        ret = self.socket_.recv_multipart()
        print ret

    def handle_module_config(self, mod):
        print "Running 'module_config' for module", mod
        self.socket_.send_multipart(["MODULE_CONFIG", mod])
        ret = self.socket_.recv_multipart()
        print ret

    def sync_from(self, endpoint):
        print "Will ask Leosac to sync from {", endpoint, "}"
        self.socket_.send_multipart(["SYNC_FROM", endpoint])
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

    
    time.sleep(1)
    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
