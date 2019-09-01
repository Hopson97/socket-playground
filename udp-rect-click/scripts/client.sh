#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/udp-rects client $2
else
    ./bin/debug/udp-rects client $1
fi