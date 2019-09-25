#!/bin/bash
cd submodules/Ardwiino
git fetch
git reset --hard origin/master
make build-all
cp -rf output/* ../../firmware/
cd ../../
git add .
git commit -m "Update Ardwiino"