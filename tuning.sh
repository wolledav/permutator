

cd $SCRATCHDIR
cp /storage/praha1/home/pazoudav/permutator ./ -r
PATH=$PATH:/storage/praha1/home/pazoudav/Rpackages/irace/bin/
module load r/4.0.0-gcc
export R_LIBS_USER="/storage/praha1/home/pazoudav/Rpackages"
Rscript -e 'library("irace")'

cd permutator/
module add cmake
cmake -S . -B"./build"
cmake --build build
cd tuning_setups/tuning_EVRP/
