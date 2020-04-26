#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/master
git submodule init
git submodule update
git describe --abbrev=0 --tags > output/version
make clean
rm -r src/uno/usb/obj
rm -r src/uno/main/obj
rm -r src/micro/obj
rm -r src/uno/usb/bin
rm -r src/uno/main/bin
rm -r src/micro/bin
make build-all
cp -rf output/* ../../firmware/
cd ../../
git add .
git commit -m "Update Ardwiino"
