#!/bin/bash

rm -rf -d lib/sabre
cd lib
git clone https://github.com/Hopson97/sabre
mv sabre sabret
cd sabret
sh build.sh
mv sabre/ ../sabre
cd ..
rm -rf -d sabret
