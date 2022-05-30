#!/bin/bash

TIME_LIMIT=2400
QAP_DATASETS=("../data/qap-taillard/tai20a.txt" "../data/qap-taillard/tai20b.dat" "../data/qap-taillard/tai25a.txt")
#CVRP_DATASETS=("../data/cvrp-augerat-P/P-n055-k08.xml" "../data/cvrp-augerat-P/P-n070-k10.xml" "../data/cvrp-uchoa/X-n106-k14.xml")

TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/$TIMESTAMP"

echo $LOGDIR
mkdir "$LOGDIR"

for data in "${CVRP_DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  echo "Testing dataset ${data}"
  ../bin/cvrp_meta_normal -d $data -t $TIME_LIMIT -o "../log/$LOGDIR/cvrp-normal-${filename}.out"
  ../bin/cvrp_meta_rnd -d $data -t $TIME_LIMIT -o "../log/$LOGDIR/cvrp-rnd-${filename}.out"
done

for data in "${QAP_DATASETS[@]}"
do
  filename=$(basename -s ".txt" -s ".dat" $data )
  filename=$(basename -s ".txt" $filename )
  echo "Testing dataset ${data}"
  ../bin/qap_meta_normal -d $data -t $TIME_LIMIT -o "../log/$LOGDIR/qap-normal-${filename}.out"
  ../bin/qap_meta_rnd -d $data -t $TIME_LIMIT -o "../log/$LOGDIR/qap-rnd-${filename}.out"
done

