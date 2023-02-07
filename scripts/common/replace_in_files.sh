strings=(
#  's/two_opt/twoOpt/g'
#  's/exchange_ids/exchangeIds/g'
#  's/exchange_n_ids/exchangeNIds/g'
#  's/rexchange/revExchange/g'
#  's/centered_exchange/centExchange/g'
#  's/rrelocate/revRelocate/g'
#  's/move_all_by/moveAll/g'
#  's/random_double_bridge/randomDoubleBridge/g'
#  's/double_bridge/doubleBridge/g'
#  's/random_move_all/randomMoveAll/g'
#  's/random_move/randomMove/g'
#  's/random_swap/randomSwap/g'
#  's/random_replicate/randomReplicate/g'

#"s/burma14-2.20/burma14-3.1/g"
#"s/ulysses22-3.80/ulysses22-5.5/g"
#"s/berlin52-5.00/berlin52-10.4/g"
#"s/berlin52-8.30/berlin52-13.2/g"
#"s/eil101-18.00/eil101-27.5/g"
#"s/gr202-50.00/gr202-67.3/g"
#"s/lin318-70.00/lin318-99.3/g"
#"s/fl417-130.00/fl417-160.6/g"
#"s/d657-290.00/d657-322.7/g"
#"s/rat783-460.00/rat783-481.4/g"
#"s/vm1084-840.00/vm1084-848.9/g"

# TODO run in data
"s/scp/TSPSD/"
"s/SCP/TSPSD/"
"s/TSP-SD/TSPSD/"
"s/wcp/wTSPSD/"
"s/wcp/wTSPSD/"
"s/weak_TSPSD/wTSPSD/"
"s/cvrp/CVRP/"
"s/npfs/NPFS/"
"s/qap/QAP/"
"s/sudoku/SUDOKU/"
)

for string in "${strings[@]}"
do
  echo "$string"
  find . -type f -exec sed -i "$string" {} +
done

