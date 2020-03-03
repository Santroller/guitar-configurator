#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/master
git submodule init
git submodule update
git describe --abbrev=0 --tags > output/version
make build-all
cp -rf output/* ../../firmware/
cd ../../
git add .
git commit -m "Update Ardwiino"