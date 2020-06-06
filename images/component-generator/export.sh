#!/bin/bash
folder=../Mouse
rm -r $folder/components/*
./svg-objects-export.py -P "" -p '^g.*' -d $folder/components/ $folder/Base.svg -t plain-svg