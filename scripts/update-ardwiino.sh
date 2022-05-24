#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/v1
git submodule init
git submodule update
git describe --abbrev=0 --tags > ../../firmware/version
rm -r build
mkdir -p build
cd build
cmake .. -DBOARD=
make -j`nproc`

cp -rf firmware/*.hex ../../../firmware

for filename in ../submodules/pico-sdk/src/boards/include/boards/*.h; do
    filename=`basename -s .h ${filename}`
    if [ "$filename" != "none" ]; then
        cmake .. -DBOARD=${filename}
        make -j`nproc`
        cp -rf firmware/*.uf2 ../../../firmware
    fi
done
cmake .. -DBOARD=

cd ../../../