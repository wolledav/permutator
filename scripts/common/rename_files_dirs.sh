#!/bin/bash

array1=(
#"burma14-2.20"
#"ulysses22-3.80"
#"berlin52-5.00"
#"berlin52-8.30"
#"eil101-18.00"
#"gr202-50.00"
#"lin318-70.00"
#"fl417-130.00"
#"d657-290.00"
#"rat783-460.00"
#"vm1084-840.00"
#"scp"
#"SCP"
#"TSP-SD"
#"wcp"
#"WCP"
#"weak_TSPSD"
#"cvrp"
#"npfs"
#"qap"
#"sudoku"
)

array2=(
#"s/burma14-2.20/burma14-3.1/"
#"s/ulysses22-3.80/ulysses22-5.5/"
#"s/berlin52-5.00/berlin52-10.4/"
#"s/berlin52-8.30/berlin52-13.2/"
#"s/eil101-18.00/eil101-27.5/"
#"s/gr202-50.00/gr202-67.3/"
#"s/lin318-70.00/lin318-99.3/"
#"s/fl417-130.00/fl417-160.6/"
#"s/d657-290.00/d657-322.7/"
#"s/rat783-460.00/rat783-481.4/"
#"s/vm1084-840.00/vm1084-848.9/"
#"s/scp/TSPSD/"
#"s/SCP/TSPSD/"
#"s/TSP-SD/TSPSD/"
#"s/wcp/wTSPSD/"
#"s/wcp/wTSPSD/"
#"s/weak_TSPSD/wTSPSD/"
#"s/cvrp/CVRP/"
#"s/npfs/NPFS/"
#"s/qap/QAP/"
#"s/sudoku/SUDOKU/"
)

for i in "${!array1[@]}"; do
  echo "${array1[i]}"
  find . -type d -name "*${array1[i]}*" -exec rename "${array2[i]}" {} +
  find . -type f -name "*${array1[i]}*" -exec rename "${array2[i]}" {} +
done

#find . -type f -name "*ma14-2.20*" | xargs -r rename "s/burma14-3.1/ma14-3.10/"


