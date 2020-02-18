#!/bin/sh

# build domsec
cd ../chariot-semantic-domains
mkdir -p build
cd build
cmake ..
make -j 4

# build armsec
cd ../../chariot-formal-decoder-armv7
cd armsec
autoreconf
mkdir -p build
cd build
../configure --prefix=$PWD
make -j 4

# connect armsec and domsec
cd ../../src
mkdir -p armsec
cd armsec
if [ ! -f top_thumb.hh ]; then
    ln -s $PWD../../armsec/build/armsec/top_thumb.hh top_thumb.hh
fi
if [ ! -f top_thumb.tcc ]; then
    ln -s $PWD../../armsec/build/armsec/top_thumb.hh top_thumb.tcc
fi
cd ..
make -j 4

# build memsec, back to home
cd ../../chariot-formal-contract-checker
mkdir -p build
cd build
cmake ..
make -j 4
cd ..

