#!/bin/bash
# run from permutator/

CMD="python3 ./tools/tspsd/draw_wcp.py"
INSTANCES=("./data/tspsd/berlin52_random/")
SOLUTIONS="./log/wcp-meta/berlin52_random_10min/"
OUTDIR="./figures/wcp/berlin52_random_10min_solved/"

#CMD="python3 ./tools/tspsd/draw_scp.py"
#INSTANCES=("./data/tspsd/berlin52_random/")
#SOLUTIONS="./log/scp-meta/berlin52_random_10min/"
#OUTDIR="./figures/scp/berlin52_random_10min_solved/"

cmake -E make_directory "$OUTDIR"

for instance in "$INSTANCES"*
do
  echo $instance
  filename=$(basename $instance ".json")
  solution=$SOLUTIONS$filename".out"
  $CMD -i $instance -o $OUTDIR -s $solution
done
pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR"all.pdf"
