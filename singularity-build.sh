#!/bin/bash

echo "Building Singularity Image..." > ./output/build_log.txt 2>&1
sudo singularity build -F si-roof.sif singconf.def >> ./output/build_log.txt 2>&1

export RETVAL=$?
if [ $RETVAL -ne 0 ]; then
    echo "Error while building image"
fi