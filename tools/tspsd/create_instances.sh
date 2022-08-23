#!/bin/bash

input_file='./data/tsp/berlin52.json'

cmd='./tools/tspsd/tsp2tspsd_closest.py'
output_dir='./data/tspsd/berlin52_closest/'

# cmd='./tools/tspsd/tsp2tspsd_random.py'
# output_dir='./data/tspsd/berlin52_random/'


for k in {1..51}
do
  python3 $cmd $input_file $output_dir $k
done
