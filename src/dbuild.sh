#!/bin/sh
mkdir -p build
cd build
cmake ..
make install DESTDIR=.
./usr/local/bin/Command_Runner
