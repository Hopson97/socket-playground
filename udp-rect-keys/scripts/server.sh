#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/udp-rects server _
else
    ./bin/debug/udp-rects server _
fi