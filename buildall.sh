#!/bin/sh

set -e

cd ../libgc
make clean
make linux

cd BASIClib
make clean
make linux

cd ../tools/testlib
make clean
make linux

echo "All done!"

