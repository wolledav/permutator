#!/bin/bash
# run from permutator/

CMD="python3 ./tools/tspsd/plot_progress.py"

# TSPSD, closest, 1 min
#SOLUTIONS=("./log/TSPSD-meta/berlin52_closest_1min/")
#OUTDIR="./figures/TSPSD/berlin52_closest_1min_progress/"
#TYPE="TSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"


# TSPSD, closest, 10 min
#SOLUTIONS=("./log/TSPSD-meta/berlin52_closest_10min/")
#OUTDIR="./figures/TSPSD/berlin52_closest_10min_progress/"
#TYPE="TSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## TSPSD, random, 1 min
#SOLUTIONS=("./log/TSPSD-meta/berlin52_random_1min/")
#OUTDIR="./figures/TSPSD/berlin52_random_1min_progress/"
#TYPE="TSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"

# TSPSD, random, 10 min
SOLUTIONS=("./log/TSPSD-meta/berlin52_random_10min/")
OUTDIR="./figures/TSPSD/berlin52_random_10min_progress/"
TYPE="TSPSD"

cmake -E make_directory "$OUTDIR"
for SOLUTION in "$SOLUTIONS"*
do
  echo $SOLUTION
  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
done
pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
#
#
#
#
## wTSPSD, closest, 1 min
#SOLUTIONS=("./log/wTSPSD-meta/berlin52_closest_1min/")
#OUTDIR="./figures/wTSPSD/berlin52_closest_1min_progress/"
#TYPE="wTSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## wTSPSD, closest, 10 min
#SOLUTIONS=("./log/wTSPSD-meta/berlin52_closest_10min/")
#OUTDIR="./figures/wTSPSD/berlin52_closest_10min_progress/"
#TYPE="wTSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## wTSPSD, random, 1 min
#SOLUTIONS=("./log/wTSPSD-meta/berlin52_random_1min/")
#OUTDIR="./figures/wTSPSD/berlin52_random_1min_progress/"
#TYPE="wTSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## wTSPSD, random, 10 min
#SOLUTIONS=("./log/wTSPSD-meta/berlin52_random_10min/")
#OUTDIR="./figures/wTSPSD/berlin52_random_10min_progress/"
#TYPE="wTSPSD"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
