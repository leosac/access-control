#!/usr/bin/env python

import zmq

public, secret = zmq.curve_keypair()

print "Public Key: ", public
print "Private Key: ", secret

