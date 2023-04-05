#!/bin/bash

cd $SCRATCHDIR
cp /storage/praha1/home/pazoudav/permutator ./ -r

cd permutator/
module add cmake
cmake -S . -B"./build"
cmake --build build
cd tuning_setups/tuning_EVRP/

PATH=$PATH:/storage/praha1/home/pazoudav/Rpackages/irace/bin/
module load r/r-4.1.1-intel-19.0.4-ilb46fy
export R_LIBS_USER="/storage/praha1/home/pazoudav/Rpackages"
Rscript -e 'library("irace")'
irace
