#!/bin/sh

cd BASIClib
make clean
make linux

cd ../libgc
make clean
make linux

cd ../tools/testlib
make clean
make linux

echo "All done!"

