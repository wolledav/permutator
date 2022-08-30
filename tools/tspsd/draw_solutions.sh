#!/bin/bash
# run from permutator/

INSTANCES=("./data/tspsd/berlin52_closest/")
SOLUTIONS="./log/scp-meta/berlin52_closest_10min/"
OUTDIR="./figures/wcp/berlin52_closest_10min_solved/"
CMD="python3 ./tools/tspsd/draw_scp.py"

cmake -E make_directory "$OUTDIR"

for instance in "$INSTANCES"*
do
  echo $instance
  filename=$(basename $instance ".json")
  solution=$SOLUTIONS$filename".out"
  $CMD -i $instance -o $OUTDIR -s $solution
done