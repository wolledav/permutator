strings=(
#  's/two_opt/twoOpt/g'
  's/exchange_ids/exchangeIds/g'
  's/exchange_n_ids/exchangeNIds/g'
  's/rexchange/revExchange/g'
  's/centered_exchange/centExchange/g'
  's/rrelocate/revRelocate/g'
  's/move_all_by/moveAll/g'
  's/random_double_bridge/randomDoubleBridge/g'
  's/double_bridge/doubleBridge/g'
  's/random_move_all/randomMoveAll/g'
  's/random_move/randomMove/g'
  's/random_swap/randomSwap/g'
  's/random_replicate/randomReplicate/g'
)

for string in "${strings[@]}"
do
  echo "$string"
  find . -type f -exec sed -i "$string" {} +
done

