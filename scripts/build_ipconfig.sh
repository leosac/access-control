#!/bin/bash

#Args:
#   1:dhcp enabled
#   2:target config
#   3:interface

HEADER="# This file was auto-generated
auto lo
iface lo inet loopback
"

echo $HEADER > $2
echo "auto $3" >> $2
echo "allow-hotplug $3" >> $2

if [[ "$1" == 1 ]]; then
    echo "iface $3 inet dhcp" >> $2
else
    echo "iface $3 inet static" >> $2
    echo "    address $4" >> $2
    echo "    netmask $5" >> $2
fi
