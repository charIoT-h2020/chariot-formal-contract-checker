#!/bin/sh

# build domsec
cd ../chariot-semantic-domains
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 4

# build armsec
cd ../../chariot-formal-decoder-armv7
cd armsec
# touch ./genisslib/configure ./armsec/configure ./configure
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
    ln -s $PWD/../../armsec/build/armsec/top_thumb.hh top_thumb.hh
fi
if [ ! -f top_thumb.tcc ]; then
    ln -s $PWD/../../armsec/build/armsec/top_thumb.tcc top_thumb.tcc
fi
cd ..
make -j 4

# build mipssec
cd ../../chariot-formal-decoder-mips
autoreconf
mkdir -p build
cd build
../configure --prefix=$PWD
make -j 4
make install

# build memsec, back to home
cd ../../chariot-formal-contract-checker
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 4
cd ..

