#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/chat 
else
    ./bin/debug/chat 
fi