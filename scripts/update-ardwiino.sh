#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/master
git submodule init
git submodule update
git describe --abbrev=0 --tags > ../../firmware/version
rm -r build
mkdir -p build
./build.sh

cp -rf build/firmware/*.hex ../../firmware
cp -rf build/firmware/*.uf2 ../../firmware
cd ../../../