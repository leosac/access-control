#!/usr/bin/python2.7

import time
import sys
import zmq

class CommandHandler(object):
    def __init__(self, sock, argv):
        self.socket_ = sock
        self.argv_ = argv

    def handle_command(self, cmd):
        try:
            if (cmd == "send_card"):
                self.handle_send_card(self.argv_[3])
            elif (cmd == "send_pin_4bits"):
                self.handle_send_pin_4bits(self.argv_[3])
            else:
                print "Doing nothing..."
        except IndexError as e:
            print "Not enough argument."
            exit(1)

    def handle_send_card(self, card_id):
        print "Will send card id ", card_id
        card_id = card_id.replace(":", "")
        send_card_id(self.socket_, card_id)

    def handle_send_pin_4bits(self, pin):
        print "Will send PIN: ", pin
        for key in pin:
            #bin repr of one key. 4 bits
            binary = bin(int(key, 16))[2:].zfill(4)
            for c in binary:
                if c == "1":
                    trigger_interrupt(self.socket_, "wiegand_data_high")
                else:
                    trigger_interrupt(self.socket_, "wiegand_data_low")
            ## We need 50ms timeout to process each key, otherwise all keys look
            ## like one.
            time.sleep(0.6)

def print_usage():
    print "Usage: ./instrumentation_client ipc_endpoint command [params]"

def send_card_id(sock, card_str):
    binary_string = bin(int(card_str, 16))[2:].zfill(len(card_str) * 4)
    for c in binary_string:
        if c == "1":
            trigger_interrupt(sock, "wiegand_data_high")
        else:
            trigger_interrupt(sock, "wiegand_data_low")

def trigger_interrupt(sock, gpio_name):
    msg = [zmq.Frame("GPIO"), zmq.Frame(gpio_name), zmq.Frame("INT")]
    sock.send_multipart(msg)
    
def main():
    if len(sys.argv) < 3:
        print_usage()
        return -1
    ipc_endpoint = sys.argv[1]

    context = zmq.Context.instance()
    dealer = context.socket(zmq.DEALER)

    connect_str = "ipc://" + str(ipc_endpoint)
    dealer.connect(connect_str)
    print "Connected to " + connect_str

    ch = CommandHandler(dealer, sys.argv);
    ch.handle_command(sys.argv[2])

    context.destroy(linger=5000)
    return 0

if __name__ == "__main__":
    ret = main()
    exit(ret)
