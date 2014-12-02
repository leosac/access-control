#!/bin/bash

/sbin/ifdown -i $1 $2
/sbin/ifup -i $1 $2
