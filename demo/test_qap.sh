#!/bin/bash

DATASETS=("../data/qap-taillard/tai15a.txt" "../data/qap-taillard/tai15b.dat" "../data/qap-taillard/tai20a.txt"
          "../data/qap-taillard/tai20b.dat" "../data/qap-taillard/tai25a.txt" "../data/qap-taillard/tai25b.dat")

TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/$TIMESTAMP"

echo $LOGDIR
mkdir "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename -s ".txt" -s ".dat" $data )
  filename=$(basename -s ".txt" $filename )
  echo "Testing dataset ${data}"
  ../cmake-build-release/qap_meta -d $data -t 3600 -o "../log/$LOGDIR/meta-${filename}.out"
  ../cmake-build-release/qap_ilp -d $data -t 3600 -o "../log/$LOGDIR/ilp-${filename}.out"
done
