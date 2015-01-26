#!/usr/bin/env python

import struct
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
            return self.handle_module_list();
        elif (cmd == "module_config"):
            return self.handle_module_config(self.argv_[self.argv_offset])
        elif (cmd == "sync_from"):
            return self.sync_from(self.argv_[self.argv_offset])
        else:
            print "Non-handled command: ", cmd

    def handle_module_list(self):
        print "Running 'module_list;"
        self.socket_.send("MODULE_LIST")
        ret = self.socket_.recv_multipart()
        return ret

    def handle_module_config(self, mod):
        print "Running 'module_config' for module", mod
        # we prefer text (xml) config format.
        cfg_format = struct.pack("!B", 1)
        self.socket_.send_multipart(["MODULE_CONFIG", mod, cfg_format])
        ret = self.socket_.recv_multipart()
        return ret

    def sync_from(self, endpoint):
        print "Will ask Leosac to sync from {", endpoint, "}"
        ## not autocommit / autosave of configuration.
        autocommit = struct.pack("!B", 0)
        self.socket_.send_multipart(["SYNC_FROM", endpoint, autocommit])
        ret = self.socket_.recv_multipart()
        return ret

def print_usage():
    print "Usage: ./remote_control tcp_endpoint server_key command [params]"

def run(user_params):
    if len(user_params) < 4:
        print_usage()
        return -1
    tcp_endpoint = user_params[1]
    server_key = user_params[2]

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

    ch = CommandHandler(dealer, user_params
);
    ret = ch.handle_command(user_params[3])
    
    time.sleep(1)
    context.destroy(linger=5000)
    return ret

def main():
    ret = run(sys.argv)
    print "Result = ", ret

if __name__ == "__main__":
    ret = main()
    exit(ret)
