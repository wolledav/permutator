#!/bin/bash
# run from permutator/

INSTANCES=("./data/tspsd/berlin52_random/")
SOLUTIONS="./log/scp-meta/berlin52_random/"
OUTDIR="./figures/berlin52_random_solved/"
CMD="python3 ./tools/tspsd/draw_tspsd.py"

cmake -E make_directory "$OUTDIR"

for instance in "$INSTANCES"*
do
  echo $instance
  filename=$(basename $instance ".json")
  solution=$SOLUTIONS$filename".out"
  $CMD -i $instance -o $OUTDIR -s $solution
done