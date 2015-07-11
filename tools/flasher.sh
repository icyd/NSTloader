#!/bin/bash

path="/mnt/data/github/NSTloader/tools/"
oldpath=$(pwd)

cd $path
./nstbFlasher.py $1 $2 $oldpath/$3
