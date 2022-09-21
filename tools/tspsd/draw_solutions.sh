#!/bin/bash
# run from permutator/

CMD="python3 ./tools/tspsd/draw_cp.py"

# distance, scp
#INSTANCES=("./data/tspsd/berlin52_distance_v2/")
#SOLUTIONS="./log/scp-meta/berlin52_distance_v2_3600s/"
#OUTDIR="./figures/scp/berlin52_distance_v2_3600s_solved/"
#MERGED="berlin52_distance_v2_3600s_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED

## distance, scp
#INSTANCES=("./data/tspsd/eil101_distance/")
#SOLUTIONS="./log/scp-meta/eil101_distance_3600s/"
#OUTDIR="./figures/scp/eil101_distance_3600s_solved/"
#MERGED="eil101_distance_3600s_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
## distance, scp
#INSTANCES=("./data/tspsd/ch150_distance/")
#SOLUTIONS="./log/scp-meta/ch150_distance_3600s/"
#OUTDIR="./figures/scp/ch150_distance_3600s_solved/"
#MERGED="ch150_distance_3600s_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED

# distance, scp
INSTANCES=("./data/tspsd/a280_distance/")
SOLUTIONS="./log/scp-meta/a280_distance_3600s/"
OUTDIR="./figures/scp/a280_distance_3600s_solved/"
MERGED="a280_distance_3600s_all.pdf"

cmake -E make_directory "$OUTDIR"
for instance in "$INSTANCES"*
do
  filename=$(basename $instance ".json")
  solution=$SOLUTIONS$filename".out"
  $CMD -i $instance -o $OUTDIR -s $solution
done
pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED



## distance, scp
#INSTANCES=("./data/tspsd/berlin52_distance/")
#SOLUTIONS="./log/scp-meta/berlin52_distance_10min/"
#OUTDIR="./figures/scp/berlin52_distance_10min_solved/"
#MERGED="berlin52_distance_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
## x_closest_edges, scp
#INSTANCES=("./data/tspsd/berlin52_x_closest_edges/")
#SOLUTIONS="./log/scp-meta/berlin52_x_closest_edges_10min/"
#OUTDIR="./figures/scp/berlin52_x_closest_edges_10min_solved/"
#MERGED="berlin52_x_closest_edges_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
#
## distance, wcp
#INSTANCES=("./data/tspsd/berlin52_distance/")
#SOLUTIONS="./log/wcp-meta/berlin52_distance_10min/"
#OUTDIR="./figures/wcp/berlin52_distance_10min_solved/"
#MERGED="berlin52_distance_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
## x_closest_edges, wcp
#INSTANCES=("./data/tspsd/berlin52_x_closest_edges/")
#SOLUTIONS="./log/wcp-meta/berlin52_x_closest_edges_10min/"
#OUTDIR="./figures/wcp/berlin52_x_closest_edges_10min_solved/"
#MERGED="berlin52_x_closest_edges_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED


## random, wcp
#INSTANCES=("./data/tspsd/berlin52_random/")
#SOLUTIONS="./log/wcp-meta/berlin52_random_10min/"
#OUTDIR="./figures/wcp/berlin52_random_10min_solved/"
#MERGED="berlin52_random_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
## closest, wcp
#INSTANCES=("./data/tspsd/berlin52_closest/")
#SOLUTIONS="./log/wcp-meta/berlin52_closest_10min/"
#OUTDIR="./figures/wcp/berlin52_closest_10min_solved/"
#MERGED="berlin52_closest_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
#
## random, scp
#INSTANCES=("./data/tspsd/berlin52_random/")
#SOLUTIONS="./log/scp-meta/berlin52_random_10min/"
#OUTDIR="./figures/scp/berlin52_random_10min_solved/"
#MERGED="berlin52_random_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
#
## closest, scp
#INSTANCES=("./data/tspsd/berlin52_closest/")
#SOLUTIONS="./log/scp-meta/berlin52_closest_10min/"
#OUTDIR="./figures/scp/berlin52_closest_10min_solved/"
#MERGED="berlin52_closest_10min_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED
