#!/bin/sh
rm -rf build &&
mkdir build && 
cd build &&
cmake .. &&
make -j12 &&
./P3
