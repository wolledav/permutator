#!/bin/bash
# run from permutator/

CMD="python3 ./tools/tspsd/plot_progress.py"

# SCP, closest, 1 min
SOLUTIONS=("./log/scp-meta/berlin52_closest_1min/")
OUTDIR="./figures/scp/berlin52_closest_1min_progress/"
TYPE="SCP"

#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"


# SCP, closest, 10 min
#SOLUTIONS=("./log/scp-meta/berlin52_closest_10min/")
#OUTDIR="./figures/scp/berlin52_closest_10min_progress/"
#TYPE="SCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## SCP, random, 1 min
#SOLUTIONS=("./log/scp-meta/berlin52_random_1min/")
#OUTDIR="./figures/scp/berlin52_random_1min_progress/"
#TYPE="SCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## SCP, random, 10 min
#SOLUTIONS=("./log/scp-meta/berlin52_random_10min/")
#OUTDIR="./figures/scp/berlin52_random_10min_progress/"
#TYPE="SCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
#
#
#
#
## WCP, closest, 1 min
#SOLUTIONS=("./log/wcp-meta/berlin52_closest_1min/")
#OUTDIR="./figures/wcp/berlin52_closest_1min_progress/"
#TYPE="WCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## WCP, closest, 10 min
#SOLUTIONS=("./log/wcp-meta/berlin52_closest_10min/")
#OUTDIR="./figures/wcp/berlin52_closest_10min_progress/"
#TYPE="WCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## WCP, random, 1 min
#SOLUTIONS=("./log/wcp-meta/berlin52_random_1min/")
#OUTDIR="./figures/wcp/berlin52_random_1min_progress/"
#TYPE="WCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
#
## WCP, random, 10 min
#SOLUTIONS=("./log/wcp-meta/berlin52_random_10min/")
#OUTDIR="./figures/wcp/berlin52_random_10min_progress/"
#TYPE="WCP"
#
#cmake -E make_directory "$OUTDIR"
#for SOLUTION in "$SOLUTIONS"*
#do
#  echo $SOLUTION
#  $CMD -i $SOLUTION -o $OUTDIR -t $TYPE
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
