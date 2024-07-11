#!/bin/bash

echo "Binding Volumes..." > ./output/exec_log.txt 2>&1
export SINGULARITY_BIND=./assets:/assets,./output:/output

echo "Running Script..." >> ./output/exec_log.txt 2>&1
singularity exec si-roof.sif /build/RoofBuilder >> ./output/exec_log.txt 2>&1

echo "Removing temp files..." >> ./output/exec_log.txt 2>&1
rm -rf ./assets/temp/*