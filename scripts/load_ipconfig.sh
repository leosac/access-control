#!/bin/bash

ifdown -i $1 $2
ifup -i $1 $2
