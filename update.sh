#!/bin/sh

# build domsec
cd ../chariot-semantic-domains/build
git pull --ff-only
cd build
make -j 4

# build armsec
cd ../../chariot-formal-decoder-armv7
git pull --ff-only
cd armsec/build
make -j 4

# connect armsec and domsec
cd ../../src
make -j 4

# build memsec, back to home
cd ../../chariot-formal-contract-checker
git pull --ff-only
cd build
make -j 4
cd ..

