#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/chat server _
else
    ./bin/debug/chat server _
fi