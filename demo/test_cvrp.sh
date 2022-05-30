#!/bin/bash

DATASETS=("../data/cvrp-augerat-P/P-n055-k08.xml" "../data/cvrp-augerat-P/P-n070-k10.xml"
"../data/cvrp-uchoa/X-n106-k14.xml" "../data/cvrp-uchoa/X-n223-k34.xml" "../data/cvrp-uchoa/X-n351-k40.xml")

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  echo "Testing dataset ${data}"
  ../cmake-build-release/cvrp_meta -d $data -t 3600 -o "../log/meta-${filename}.out"
  ../cmake-build-release/cvrp_ilp -d $data -t 3600 -o "../log/ilp-${filename}.out"
done