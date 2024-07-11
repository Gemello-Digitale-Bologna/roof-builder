#!/bin/bash

# echo "Cleaning target"
#cmake --build build --target clean

echo "Configuring CMake..."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release > ./output/build_log.txt 2>&1

echo "Building..."
if cmake --build build >> ./output/build_log.txt 2>&1; then
    echo "Running..."
    ./build/RoofBuilder > ./output/exec_log.txt 2>&1
    echo "Removing temp files..."
    rm -rf ./assets/temp/*
else
    echo "Building failed."
fi