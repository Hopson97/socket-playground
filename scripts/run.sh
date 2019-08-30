#!/bin/bash

if [ "$1" = "release" ]
then
    if [ "$2" = "server" ]
    then
        ./bin/release/chat server _
    else
        ./bin/release/chat client x
    fi
else
    if [ "$2" = "server" ]
    then
        ./bin/debug/chat server _
    else
        ./bin/debug/chat client x
    fi
fi