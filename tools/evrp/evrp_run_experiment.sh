files=(
    "evrp-E-n22-k4.json 3"
    "evrp-E-n23-k3.json 3"
    "evrp-E-n30-k3.json 3"
    "evrp-E-n33-k4.json 4"
    "evrp-E-n51-k5.json 6",
    "evrp-E-n76-k7.json 9",
    "evrp-E-n101-k8.json 11",
    "evrp-X-n143-k7.json 31",
    "evrp-X-n214-k11.json 47",
    # "evrp-X-n351-k40.json 81",
    # "evrp-X-n459-k26.json 101",
    # "evrp-X-n573-k30.json 122",
    # "evrp-X-n685-k75.json 150",
    # "evrp-X-n749-k98.json 165",
    # "evrp-X-n819-k171.json 179",
    # "evrp-X-n916-k207.json 196",
    # "evrp-X-n1001-k43.json 321"
)

use_repair="no_repair"


config="./configs/EVRP_"$use_repair".json"

for ((i = 0; i < ${#files[@]}; i++))
do
    set -- ${files[$i]}
    data="./data/evrp/json/"$1
    stats="./data/evrp/stats/stats-"$use_repair"-"$(basename $1 .json)".txt"
    timeout=$2

    rm $stats -f
    touch $stats

    for ((seed = 1; seed <= 20; seed++))
    do
        ./build/evrp_meta -c $config -d $data -s $seed -t $timeout >> $stats
    done

done

