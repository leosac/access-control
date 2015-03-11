#!/bin/bash

/sbin/ifdown -i $1 $2
sleep 1s;
/sbin/ifup -i $1 $2
