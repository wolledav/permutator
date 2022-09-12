#!/bin/bash

# Closest
input_file='./data/tsp/berlin52.json'
output_dir='./data/tspsd/berlin52_closest/'
cmd='./tools/tspsd/tsp2tspsd_closest.py'
mkdir -p $output_dir
for k in {1..51}
do
  python3 $cmd $input_file $output_dir $k
done

input_file='./data/tsp/vm1084.json'
output_dir='./data/tspsd/vm1084_closest/'
mkdir -p $output_dir
for k in {0..5}
do
  python3 $cmd $input_file $output_dir $k
done

# Random
input_file='./data/tsp/berlin52.json'
output_dir='./data/tspsd/berlin52_random/'
cmd='./tools/tspsd/tsp2tspsd_random.py'
mkdir -p $output_dir
for k in {1..51}
do
  python3 $cmd $input_file $output_dir $k
done

input_file='./data/tsp/vm1084.json'
output_dir='./data/tspsd/vm1084_random/'
mkdir -p $output_dir
for k in {0..5}
do
  python3 $cmd $input_file $output_dir $k
done

