#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<fex_file>"
    echo ""
    echo "Takes fex file in input, compiles it and installs it on Cubieboard sdcard"
    echo ""
    echo "Example:" $0 "test.fex"
    exit 1
fi

TARGET_PATH="/mnt/sdcard"
TARGET_DEVICE="/dev/mmcblk0p1"

#Mount fs
sudo mkdir -p $TARGET_PATH
sudo mount $TARGET_DEVICE $TARGET_PATH

#Compile
rm script.bin
fexc -vq -I fex -O bin $1 script.bin

#Backup existing script.bin
cp $TARGET_PATH/script.bin script.bin.old

#Copy to target
sudo cp script.bin $TARGET_PATH/script.bin

#Un-mount sdcard
sudo umount $TARGET_PATH
