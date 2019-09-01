#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/chat client $2
else
    ./bin/debug/chat client $1
fi