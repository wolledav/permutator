#!/bin/bash
# run from permutator/

CMD="python3 ./tools/tspsd/draw_cp.py"

# distance, wTSPSD
INSTANCES=("./data/tspsd/berlin52_closest/")
SOLUTIONS="./log/wTSPSD-meta/berlin52_closest_600s_ws/"
OUTDIR="./figures/wTSPSD/berlin52_closest_600s_ws_solved/"
MERGED="berlin52_closest_600s_ws_all.pdf"

cmake -E make_directory "$OUTDIR"
for instance in "$INSTANCES"*
do
  filename=$(basename $instance ".json")
  solution=$SOLUTIONS$filename".out"
  $CMD -i $instance -o $OUTDIR -s $solution
done
pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED


# distance, TSPSD
#INSTANCES=("./data/tspsd/berlin52_distance_v2/")
#SOLUTIONS="./log/TSPSD-meta/berlin52_distance_v2_3600s/"
#OUTDIR="./figures/TSPSD/berlin52_distance_v2_3600s_solved/"
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

# distance, wTSPSD
#INSTANCES=("./data/tspsd/eil101_distance/")
#SOLUTIONS="./log/wTSPSD-meta/eil101_distance_3600s_ws/"
#OUTDIR="./figures/wTSPSD/eil101_distance_3600s_ws_solved/"
#MERGED="eil101_distance_3600s_ws_all.pdf"
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
## distance, wTSPSD
#INSTANCES=("./data/tspsd/ch150_distance/")
#SOLUTIONS="./log/wTSPSD-meta/ch150_distance_3600s_ws/"
#OUTDIR="./figures/wTSPSD/ch150_distance_3600s_ws_solved/"
#MERGED="ch150_distance_3600s_ws_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED

# distance, wTSPSD
#INSTANCES=("./data/tspsd/a280_distance/")
#SOLUTIONS="./log/wTSPSD-meta/a280_distance_3600s_ws/"
#OUTDIR="./figures/wTSPSD/a280_distance_3600s_ws_solved/"
#MERGED="a280_distance_3600s_ws_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED

# distance, wTSPSD
#INSTANCES=("./data/tspsd/vm1084_closest/")
#SOLUTIONS="./log/wTSPSD-meta/vm1084_closest_3600s_ws/"
#OUTDIR="./figures/wTSPSD/vm1084_closest_3600s_ws_solved/"
#MERGED="vm1084_closest_3600s_ws_all.pdf"
#
#cmake -E make_directory "$OUTDIR"
#for instance in "$INSTANCES"*
#do
#  filename=$(basename $instance ".json")
#  solution=$SOLUTIONS$filename".out"
#  $CMD -i $instance -o $OUTDIR -s $solution
#done
#pdfunite $(ls -v -d $OUTDIR/*) $OUTDIR$MERGED


## distance, TSPSD
#INSTANCES=("./data/tspsd/berlin52_distance/")
#SOLUTIONS="./log/TSPSD-meta/berlin52_distance_10min/"
#OUTDIR="./figures/TSPSD/berlin52_distance_10min_solved/"
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
## x_closest_edges, TSPSD
#INSTANCES=("./data/tspsd/berlin52_x_closest_edges/")
#SOLUTIONS="./log/TSPSD-meta/berlin52_x_closest_edges_10min/"
#OUTDIR="./figures/TSPSD/berlin52_x_closest_edges_10min_solved/"
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
## distance, wTSPSD
#INSTANCES=("./data/tspsd/berlin52_distance/")
#SOLUTIONS="./log/wTSPSD-meta/berlin52_distance_10min/"
#OUTDIR="./figures/wTSPSD/berlin52_distance_10min_solved/"
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
## x_closest_edges, wTSPSD
#INSTANCES=("./data/tspsd/berlin52_x_closest_edges/")
#SOLUTIONS="./log/wTSPSD-meta/berlin52_x_closest_edges_10min/"
#OUTDIR="./figures/wTSPSD/berlin52_x_closest_edges_10min_solved/"
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


## random, wTSPSD
#INSTANCES=("./data/tspsd/berlin52_random/")
#SOLUTIONS="./log/wTSPSD-meta/berlin52_random_10min/"
#OUTDIR="./figures/wTSPSD/berlin52_random_10min_solved/"
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
## closest, wTSPSD
#INSTANCES=("./data/tspsd/berlin52_closest/")
#SOLUTIONS="./log/wTSPSD-meta/berlin52_closest_10min/"
#OUTDIR="./figures/wTSPSD/berlin52_closest_10min_solved/"
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
## random, TSPSD
#INSTANCES=("./data/tspsd/berlin52_random/")
#SOLUTIONS="./log/TSPSD-meta/berlin52_random_10min/"
#OUTDIR="./figures/TSPSD/berlin52_random_10min_solved/"
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
## closest, TSPSD
#INSTANCES=("./data/tspsd/berlin52_closest/")
#SOLUTIONS="./log/TSPSD-meta/berlin52_closest_10min/"
#OUTDIR="./figures/TSPSD/berlin52_closest_10min_solved/"
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
