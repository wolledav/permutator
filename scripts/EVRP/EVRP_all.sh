
CMD="./cmake-build-release/Debug/EVRP_meta.exe"
TEST_TIMEOUT=300
DATASET_DIR="data_demo/datasets/"
LOG_DIR="./log/EVRP/"


D_NAME="EVRP"

D_PATH="${DATASET_DIR}${D_NAME}/"
LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s"
cmake -E make_directory "$LOG_PATH"
for FILE in "$D_PATH"*
do
    FILENAME=$(basename $FILE ".evrp")
    for i in {1..1}
    do
        $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -c "configs\EVRP.json"
    done
done