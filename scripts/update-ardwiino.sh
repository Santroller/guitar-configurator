#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/master
git submodule init
git submodule update
git describe --abbrev=0 --tags > output/version
mkdir -p build
cd build
rm -rf firmware
cmake ..
make clean
make -j`nproc`
cp -rf firmware/*.hex ../../../firmware
cp -rf firmware/*.uf2 ../../../firmware
cd ../../../