#!/usr/bin/env python

import sys
import zmq

def print_usage():
    print "Usage: ./intrumentation_client ipc_endpoint"

def trigger_interrupt(sock, gpio_name):
    msg = [zmq.Frame("GPIO"), zmq.Frame(gpio_name), zmq.Frame("INT")]
    sock.send_multipart(msg)
    
def main():
    if len(sys.argv) < 2:
        print_usage()
        return -1
    ipc_endpoint = sys.argv[1]

    context = zmq.Context.instance()
    dealer = context.socket(zmq.DEALER)

    connect_str = "ipc://" + str(ipc_endpoint)
    dealer.connect(connect_str)
    print "Connected too " + connect_str

    trigger_interrupt(dealer, "my_lama");

    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
